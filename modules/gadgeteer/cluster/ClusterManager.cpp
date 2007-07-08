/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * VR Juggler is (C) Copyright 1998-2007 by Iowa State University
 *
 * Original Authors:
 *   Allen Bierbaum, Christopher Just,
 *   Patrick Hartling, Kevin Meinert,
 *   Carolina Cruz-Neira, Albert Baker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#include <gadget/gadgetConfig.h>

#include <iomanip>
#include <sstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>

#include <vpr/vprTypes.h>
#include <vpr/System.h>
#include <vpr/DynLoad/LibraryLoader.h>
#include <vpr/Util/FileUtils.h>

#include <jccl/Config/ConfigDefinitionPtr.h>
#include <jccl/Config/ConfigDefinition.h>
#include <jccl/RTRC/ConfigManager.h>
#include <jccl/RTRC/DependencyManager.h>

#include <gadget/gadgetParam.h>
#include <gadget/Type/DeviceFactory.h>
#include <gadget/Node.h>
#include <gadget/Util/Debug.h>
#include <gadget/Util/PathHelpers.h>
#include <gadget/Util/PluginVersionException.h>

#include <cluster/ClusterNetwork.h>
#include <cluster/ClusterPlugin.h>
#include <cluster/Packets/EndBlock.h>
#include <cluster/ClusterManager.h>


namespace fs = boost::filesystem;

namespace cluster
{
   vprSingletonImp( ClusterManager );

   /**
    * This struct implements a callable object (a functor, basically).  An
    * instance can be passed in where a boost::function1<bool, void*> is
    * expected.  In cluster::ClusterManager::configAdd(), instances are used
    * to handle version checking of cluster plug-ins via vpr::LibraryLoader.
    */
   struct VersionCheckCallable
   {
      VersionCheckCallable()
      {
      }

      /**
       * This will be invoked as a callback by methods of vpr::LibraryLoader.
       *
       * @param func A function pointer for the entry point in a dynamically
       *             loaded cluster plug-in.  This must be cast to the correct
       *             signature before being invoked.
       */
      bool operator()(void* func)
      {
         vpr::Uint32 (*version_func)();
         version_func = (vpr::Uint32 (*)()) func;

         // Call the entry point function, which, in this case, returns the
         // version of Gadgeteer against which the plug-in was compiled.
         const vpr::Uint32 plugin_gadget_ver = (*version_func)();

         if ( plugin_gadget_ver != mGadgetVersion )
         {
            std::ostringstream msg_stream;
            msg_stream << "Gadgeteer version mismatch!\n"
                       << "Cluster plug-in was compiled against Gadgeteer "
                       << "version " << plugin_gadget_ver << ",\n"
                       << "but this is Gadgeteer version " << mGadgetVersion
                       << std::endl;
            throw gadget::PluginVersionException(msg_stream.str(),
                                                 VPR_LOCATION);
         }

         return true;
      }

      static const vpr::Uint32 mGadgetVersion;
   };

   const vpr::Uint32 VersionCheckCallable::mGadgetVersion(__GADGET_version);

   /**
    * This struct implements a callable object (a functor, basically).  An
    * instance can be passed in where a boost::function1<bool, void*> is
    * expected.  In cluster::ClusterManager::configAdd(), instances are used
    * to handle dynamic loading of cluster plugins via vpr::LibraryLoader.
    */
   struct PluginInitCallable
   {
      PluginInitCallable(cluster::ClusterManager* clusterMgr)
         : mgr(clusterMgr)
      {
      }

      /**
       * This will be invoked as a callback by methods of vpr::LibraryLoader.
       *
       * @param func A function pointer for the entry point in a dynamically
       *             loaded cluster plug-in.  This must be cast to the correct
       *             signature before being invoked.
       */
      bool operator()( void* func )
      {
         void (*init_func)(ClusterManager*);

         // Cast the entry point function to the correct signature so that we
         // can call it.  All dynamically loaded plug-ins must have an entry
         // point function that takes a pointer to a cluster::ClusterManager
         // instance and returns nothing.
         init_func = (void (*)(ClusterManager*)) func;

         // Call the entry point function.
         (*init_func)( mgr );

         return true;
      }

      cluster::ClusterManager* mgr;
   };

   ClusterManager::ClusterManager()
      : mClusterActive( false )
      , mClusterReady( false )
      , mPreDrawCallCount(0)
      , mPostPostFrameCallCount(0)
   {
      mClusterNetwork = new ClusterNetwork();
      jccl::ConfigManager::instance()->addConfigElementHandler( mClusterNetwork );
      jccl::DependencyManager::instance()->registerChecker(&mDepChecker);
   }

   ClusterManager::~ClusterManager()
   {
      jccl::DependencyManager::instance()->unregisterChecker(&mDepChecker);
   }

   bool ClusterManager::isClusterReady()
   {
      // -If the cluster is active and not ready
      //   -If a StartBarrier jccl::ConfigElement does not exist
      //    -Warn the user and set cluster ready
      //   -Get new value of mClusterReady from asking all plugins
      // -Return the new mClusterReady

      vpr::Guard<vpr::Mutex> ready_guard( mClusterReadyLock );
      vpr::Guard<vpr::Mutex> active_guard( mClusterActiveLock );

      if ( mClusterActive && !mClusterReady )
      {
         if ( !jccl::ConfigManager::instance()->hasElementType( "start_barrier_plugin" ) )
         {
            vprDEBUG(gadgetDBG_RIM, vprDBG_WARNING_LVL)
               << clrOutBOLD(clrCYAN, "NOTE:") << std::endl << vprDEBUG_FLUSH;
            vprDEBUG_NEXT(gadgetDBG_RIM, vprDBG_WARNING_LVL)
               << "The start_barrier_plugin config element does not exist.\n"
               << vprDEBUG_FLUSH;
            vprDEBUG_NEXT(gadgetDBG_RIM, vprDBG_WARNING_LVL)
               << "If your application depends on each node starting at the "
               << "same time,\n" << vprDEBUG_FLUSH;
            vprDEBUG_NEXT(gadgetDBG_RIM, vprDBG_WARNING_LVL)
               << "then you should load and configure the Start Barrier "
               << "Plug-in.\n" << vprDEBUG_FLUSH;

            mClusterReady = true;
         }
      }
      // Lock it here so that we can avoid confusion in pluginsReady()
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );
      return( mClusterReady && pluginsReady() );
   }

   bool ClusterManager::pluginsReady()
   {
      // Plugins are already locked since we only call this method from
      // isClusterReady which is only called by
      // StartBarrierPlugin::postPostFrame which has already locked the list
      // of plugins.

      //vpr::Guard<vpr::Mutex> guard(mPluginsLock);

      for ( std::list<ClusterPlugin*>::iterator i = mPlugins.begin();
            i != mPlugins.end();
            ++i )
      {
         if ( !(*i)->isPluginReady() )
         {
            return false;
         }
      }

      return true;
   }

   void ClusterManager::recoverFromLostNode( gadget::Node* lost_node )
   {
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );

      for ( std::list<ClusterPlugin*>::iterator i = mPlugins.begin();
            i != mPlugins.end();
            ++i )
      {
         (*i)->recoverFromLostNode( lost_node );
      }
   }

   /**
    * Adds a new plugin to the ClusterManager.
    */
   void ClusterManager::addPlugin(ClusterPlugin* new_plugin)
   {
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );
      if ( !doesPluginExist(new_plugin) )
      {
         mPlugins.push_back( new_plugin );
         std::pair<vpr::GUID, ClusterPlugin*> p =
            std::make_pair( new_plugin->getHandlerGUID(), new_plugin );
         mPluginMap.insert( p );

         // We should do this here, but since we do not add the manager until
         // its configAdd currently you can see the problem
         jccl::ConfigManager::instance()->addConfigElementHandler( new_plugin );
         mClusterNetwork->addHandler( new_plugin );
         //We can still unregister it when removed below though
         vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_LVL )
            << clrOutBOLD( clrCYAN, "[ClusterManager] " )
            << "Adding Plugin: " << new_plugin->getPluginName()
            << std::endl << vprDEBUG_FLUSH;
      }
   }

   ClusterPlugin* ClusterManager::getPluginByGUID( const vpr::GUID& plugin_guid )
   {
      std::map<vpr::GUID, ClusterPlugin*>::const_iterator i =
         mPluginMap.find( plugin_guid );
      if( i != mPluginMap.end() )
      {
         return ( (*i).second );
      }
      return NULL;
   }

   /**
    * Removes a plugin from the ClusterManager
    */
   void ClusterManager::removePlugin( ClusterPlugin* old_plugin )
   {
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );

      mPluginMap.erase( old_plugin->getHandlerGUID() );

      std::list<ClusterPlugin*>::iterator found
         = std::find(mPlugins.begin(), mPlugins.end(), old_plugin);

      if (mPlugins.end() != found)
      {
         vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_LVL )
            << clrOutBOLD( clrCYAN, "[ClusterManager] " )
            << "Removing Plugin: " << old_plugin->getPluginName()
            << std::endl << vprDEBUG_FLUSH;
         mPlugins.erase(found);
         jccl::ConfigManager::instance()->removeConfigElementHandler(*found);
      }
   }

   /**
    * Checks if a plugin exists in the ClusterManager
    */
   bool ClusterManager::doesPluginExist( ClusterPlugin* old_plugin )
   {
      vprASSERT( mPluginsLock.test() == 1 &&
                 "mManagers Lock must be aquired before calling ClusterManager::doesManagerExist()" );

      std::list<ClusterPlugin*>::iterator found
         = std::find(mPlugins.begin(), mPlugins.end(), old_plugin);

      return (mPlugins.end() != found);
   }

   void ClusterManager::sendRequests()
   {
      // Idea is to not create frame lock if we do not need to
      bool updateNeeded = false;
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );

      vprDEBUG( gadgetDBG_NET_MGR, vprDBG_HVERB_LVL )
         << clrOutBOLD( clrCYAN,"[ClusterManager]" )
         << " sendRequests" << std::endl << vprDEBUG_FLUSH;

      for ( std::list<ClusterPlugin*>::iterator i = mPlugins.begin();
            i != mPlugins.end();
            ++i )
      {
         (*i)->sendRequests();
         updateNeeded = true;
      }

      // Only send end blocks if we really need to.
      if ( updateNeeded )
      {
         sendEndBlocksAndSignalUpdate(1);
      }
   }

   void ClusterManager::preDraw()
   {
      // Idea is to not create frame lock if we do not need to
      bool updateNeeded = false;
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );

      vprDEBUG( gadgetDBG_NET_MGR, vprDBG_HVERB_LVL )
         << clrOutBOLD( clrCYAN,"[ClusterManager]" )
         << " preDraw" << std::endl << vprDEBUG_FLUSH;

      for ( std::list<ClusterPlugin*>::iterator i = mPlugins.begin();
            i != mPlugins.end();
            ++i )
      {
         (*i)->preDraw();
         updateNeeded = true;
      }
      if ( updateNeeded )
      {
         mPreDrawCallCount++;
         sendEndBlocksAndSignalUpdate(2);
      }
   }

   void ClusterManager::postPostFrame()
   {
      // -If not running
      //   -If all plugins ready
      //     - isClusterReady


      // Idea is to not create frame lock if we do not need to
      bool updateNeeded = false;
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );

      vprDEBUG( gadgetDBG_NET_MGR, vprDBG_HVERB_LVL )
         << clrOutBOLD( clrCYAN,"[ClusterManager]" )
         << " postPostFrame" << std::endl << vprDEBUG_FLUSH;

      for ( std::list<ClusterPlugin*>::iterator i = mPlugins.begin();
            i != mPlugins.end();
            ++i )
      {
         (*i)->postPostFrame();
         updateNeeded = true;
      }
      if ( updateNeeded )
      {
         mPostPostFrameCallCount++;
         sendEndBlocksAndSignalUpdate(3);
      }
   }

   void ClusterManager::createBarrier()
   {
      vpr::Guard<vpr::Mutex> guard( mPluginsLock );

      for ( std::list<ClusterPlugin*>::iterator i = mPlugins.begin();
            i != mPlugins.end();
            ++i )
      {
         //if ((*i)->isActive())
         //{  // As soon as we find a plug-in that creates
            // a barrier, we can continue. Maybe not since
            // this will not match up on different machines
            if ( (*i)->createBarrier() )
            {
               return;
            }
         //}
      }
   }

   void ClusterManager::sendEndBlocksAndSignalUpdate( const int temp )
   {
      // If the network is not fully connected, then don't try to sync.
      // Trying to sync a network that is not fully connected can lead to
      // deadlock.
      if ( mClusterNetwork->getNumPendingNodes() > 0 )
      {
         return;
      }

      cluster::EndBlock temp_end_block(temp);

      // Used to accumulate the number of connected nodes.
      size_t num_nodes(0);

      typedef std::vector<gadget::Node*>::iterator iter_t;

      for ( iter_t i = mClusterNetwork->getNodesBegin();
            i != mClusterNetwork->getNodesEnd();
            ++i )
      {
         if ( (*i)->isConnected() )
         {
            try
            {
               // Send End Block to the node.
               (*i)->send(&temp_end_block);

               // Indicate that this node is not up to date. It will be updated
               // below.
               (*i)->setUpdated(false);

               ++num_nodes;
            }
            catch (cluster::ClusterException& ex)
            {
               vprDEBUG(gadgetDBG_RIM, vprDBG_CRITICAL_LVL)
                  << clrOutBOLD("ERROR", clrRED)
                  << ": Failed to send end block to " << (*i)->getName()
                  << std::endl << vprDEBUG_FLUSH;
               vprDEBUG_NEXT(gadgetDBG_RIM, vprDBG_CRITICAL_LVL)
                  << ex.what() << std::endl << vprDEBUG_FLUSH;
               vprDEBUG_NEXT(gadgetDBG_RIM, vprDBG_CRITICAL_LVL)
                  << "Shutting down the node." << std::endl << vprDEBUG_FLUSH;

               (*i)->shutdown();
            }
         }
      }

      gadget::Reactor& reactor = mClusterNetwork->getReactor();
      size_t completed_nodes(0);

      while ( completed_nodes != num_nodes )
      {
         std::vector<gadget::Node*> ready_nodes =
            //reactor.getReadyNodes(vpr::Interval::NoWait);
            reactor.getReadyNodes(vpr::Interval::NoTimeout);

         for ( iter_t i = ready_nodes.begin(); i != ready_nodes.end(); ++i )
         {
            // Make sure that we do not update nodes more than once simply
            // because there is data ready to read from them. This is an
            // unfortunate side effect of using a reactor in this clumsy way.
            if ( (*i)->isUpdated() )
            {
               continue;
            }

            try
            {
               // Read network packets.
               (*i)->update();
            }
            catch (cluster::ClusterException& ex)
            {
               vprDEBUG(gadgetDBG_RIM, vprDBG_CRITICAL_LVL)
                  << clrOutBOLD("ERROR", clrRED)
                  << ": Failed to complete state update for "
                  << (*i)->getName() << std::endl << vprDEBUG_FLUSH;
               vprDEBUG_NEXT(gadgetDBG_RIM, vprDBG_CRITICAL_LVL)
                  << ex.what() << std::endl << vprDEBUG_FLUSH;
               vprDEBUG_NEXT(gadgetDBG_RIM, vprDBG_CRITICAL_LVL)
                  << "Shutting down the node." << std::endl << vprDEBUG_FLUSH;

               (*i)->shutdown();
            }

            // Record completed state. This happens regardless of whether
            // the node update completed successfully. Since the node is
            // shut down if update fails, we would never get completed_nodes
            // to equal num_nodes otherwise.
            ++completed_nodes;
         }
      }
   }

   bool ClusterManager::recognizeRemoteDeviceConfig( jccl::ConfigElementPtr element )
   {
      std::string tp("input_parent");
      if ( element->getConfigDefinition()->isParent("input_device") &&
           element->getNum("device_host") > 0 )
      {
         std::string device_host =
            element->getProperty<std::string>( "device_host" );
         //std::cout << "Checking: " << element->getName() << std::endl;
         if ( !device_host.empty() )
         {
            // THIS IS A HACK: find a better way to do this
            jccl::ConfigElementPtr device_host_ptr =
               getConfigElementPointer( device_host );
            if ( device_host_ptr.get() != NULL )
            {
               std::string host_name =
                  device_host_ptr->getProperty<std::string>( "host_name" );
               if ( !cluster::ClusterNetwork::isLocalHost( host_name ) )
               {
                  return true;
               }// Device is on the local machine
            }// Could not find the deviceHost in the configuration
         }// Device is not a remote device since there is no name in the deviceHost field
      }// Else it is not a device, or does not have a deviceHost property
      return false;
   }

   bool ClusterManager::recognizeClusterManagerConfig( jccl::ConfigElementPtr element )
   {
     return( element->getID() == ClusterManager::getElementType() );
   }

   /** Adds the pending element to the configuration.
    *  @pre configCanHandle(element) == true.
    *  @return true iff element was successfully added to configuration.
    */
   bool ClusterManager::configAdd( jccl::ConfigElementPtr element )
   {
      vpr::DebugOutputGuard dbg_output( gadgetDBG_RIM, vprDBG_STATE_LVL,
                              std::string( "Cluster Manager: Adding config element.\n" ),
                              std::string( "...done adding element.\n" ) );

      vprASSERT(configCanHandle(element));
      vprASSERT(recognizeClusterManagerConfig(element));

      bool ret_val = false;      // Flag to return success

      {
         vprDEBUG( gadgetDBG_RIM,vprDBG_CONFIG_STATUS_LVL)
            << clrOutBOLD(clrCYAN,"[ClusterManager] ")
            << "Configure the Cluster: " << element->getName()
            << std::endl << vprDEBUG_FLUSH;

         // Get a list of cluster nodes to use for this cluster.
         int num_nodes = element->getNum( std::string( "cluster_node" ) );

         vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_STATUS_LVL )
            << clrOutBOLD( clrCYAN, "[ClusterManager] " )
            << "configAdd() Number of nodes: " << num_nodes
            << std::endl << vprDEBUG_FLUSH;

         for ( int i = 0 ; i < num_nodes ; ++i )
         {
            std::string new_node =
               element->getProperty<std::string>( "cluster_node" , i );
            vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_STATUS_LVL )
               << clrOutBOLD( clrCYAN, "[ClusterManager] " )
               << "configAdd() New Node Name: " << new_node
               << std::endl << vprDEBUG_FLUSH;

            jccl::ConfigElementPtr new_node_element =
               getConfigElementPointer( new_node );
            std::string new_node_hostname =
               new_node_element->getProperty<std::string>( "host_name" );

            vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_STATUS_LVL )
               << clrOutBOLD( clrCYAN, "[ClusterManager] " )
               << "configAdd() New Node Hostname: " << new_node_hostname
               << std::endl << vprDEBUG_FLUSH;

            if ( !cluster::ClusterNetwork::isLocalHost( new_node_hostname ) )
            {
               vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_STATUS_LVL )
                  << clrOutBOLD( clrCYAN, "[ClusterManager] " )
                  << "configAdd() Added Node since it is non-local"
                  << std::endl << vprDEBUG_FLUSH;

               vpr::Guard<vpr::Mutex> guard( mNodesLock );
               mNodes.push_back( new_node_hostname );
            }
         }


         // Load the plugins.

         vpr::DebugOutputGuard dbg_output( gadgetDBG_RIM, vprDBG_STATE_LVL,
                                           std::string( "Handling cluster_manager element:\n" ),
                                           std::string( "-- end state -- \n" ) );

         // Keep this up to date with the version of the element definition we're
         // expecting to handle.
         const unsigned int cur_version(2);

         // If the element version is less than cur_version, we will not try to
         // proceed.  Instead, we'll print an error message and return false so
         // that the Config Manager knows this element wasn't consumed.
         if ( element->getVersion() < cur_version )
         {
            vprDEBUG( gadgetDBG_RIM, vprDBG_CRITICAL_LVL )
               << clrOutBOLD( clrRED, "ERROR" )
               << " [gadget::ClusterManager::configAdd()]: Element named '"
               << element->getName() << "'" << std::endl << vprDEBUG_FLUSH;
            vprDEBUG_NEXT( gadgetDBG_RIM, vprDBG_CRITICAL_LVL )
               << "is version " << element->getVersion()
               << ", but we require at least version " << cur_version
               << std::endl << vprDEBUG_FLUSH;
            vprDEBUG_NEXT( gadgetDBG_RIM, vprDBG_CRITICAL_LVL )
               << "Ignoring this element and moving on." << std::endl
               << vprDEBUG_FLUSH;

            ret_val = false;
         }
         // We got the right version of the config element and can proceed.
         else
         {
            const std::string plugin_path_prop_name( "plugin_path" );
            const int path_count( element->getNum( plugin_path_prop_name ) );
            std::vector<fs::path> search_path( path_count );

            for ( unsigned int i = 0; i < search_path.size(); ++i )
            {
               std::string temp_str =
                  vpr::replaceEnvVars( element->getProperty<std::string>( plugin_path_prop_name, i ) );

               try
               {
                  search_path[i] = fs::path( temp_str, fs::native );
               }
               catch( fs::filesystem_error& fsEx )
               {
                  vprDEBUG( vprDBG_ERROR, vprDBG_CRITICAL_LVL )
                     << clrOutNORM( clrRED, "ERROR:" )
                     << "[cluster::ClusterManager::configAdd()] File system "
                     << "exception caught while converting"
                     << std::endl << vprDEBUG_FLUSH;
                  vprDEBUG_NEXT( vprDBG_ERROR, vprDBG_CRITICAL_LVL )
                     << "'" << temp_str << "'"
                     << std::endl << vprDEBUG_FLUSH;
                  vprDEBUG_NEXT( vprDBG_ERROR, vprDBG_CRITICAL_LVL )
                     << "to a Boost.Filesystem path."
                     << std::endl << vprDEBUG_FLUSH;
                  vprDEBUG_NEXT( vprDBG_ERROR, vprDBG_CRITICAL_LVL )
                     << fsEx.what() << std::endl << vprDEBUG_FLUSH;
               }
            }

            // Append a default plug-in search path to search_path.
            const fs::path default_search_dir =
               gadget::getDefaultPluginRoot() / std::string("plugins");

            vprDEBUG( gadgetDBG_RIM, vprDBG_VERB_LVL )
               << "[cluster::ClusterManager::configAdd()] Appending "
               << "default search path '"
               << default_search_dir.native_directory_string() << "'\n"
               << vprDEBUG_FLUSH;

#if defined(GADGET_DEBUG)
            // For a debug build, search in the debug subdirectory of
            // default_search_dir before looking in default_search_dir.
            search_path.push_back(default_search_dir / std::string("debug"));
#endif

            search_path.push_back( default_search_dir );

            // --- Load cluster plugin dsos -- //
            // - Load individual plugins
            const std::string plugin_prop_name( "plugin" );
            const std::string get_version_func( "getGadgeteerVersion" );
            const std::string plugin_init_func( "initPlugin" );

            int plugin_count = element->getNum( plugin_prop_name );
            std::string plugin_dso_name;

            for ( int i = 0; i < plugin_count; ++i )
            {
               plugin_dso_name =
                  element->getProperty<std::string>(plugin_prop_name, i);

               if ( !plugin_dso_name.empty() )
               {
                  vprDEBUG( gadgetDBG_RIM, vprDBG_STATE_LVL )
                     << "[cluster::ClusterManager::configAdd()] Loading "
                     << "plugin DSO '" << plugin_dso_name << "'"
                     << std::endl << vprDEBUG_FLUSH;

                  vpr::LibraryPtr dso =
                     vpr::LibraryLoader::findDSO(plugin_dso_name, search_path);

                  if ( dso.get() != NULL )
                  {
                     try
                     {
                        VersionCheckCallable version_functor;
                        vpr::LibraryLoader::callEntryPoint(dso,
                                                           get_version_func,
                                                           version_functor);

                        PluginInitCallable init_functor(this);
                        vpr::LibraryLoader::callEntryPoint(dso,
                                                           plugin_init_func,
                                                           init_functor);

                        mLoadedPlugins.push_back(dso);
                     }
                     catch (gadget::PluginVersionException& ex)
                     {
                        vprDEBUG(gadgetDBG_INPUT_MGR, vprDBG_CRITICAL_LVL)
                           << clrOutBOLD(clrRED, "ERROR")
                           << ": Version mismatch while loading cluster "
                           << "plug-in DSO '" << plugin_dso_name << "'\n"
                           << vprDEBUG_FLUSH;
                        vprDEBUG_NEXT(gadgetDBG_INPUT_MGR, vprDBG_CRITICAL_LVL)
                           << "This plug-in will not be usable.\n"
                           << vprDEBUG_FLUSH;
                        vprDEBUG(gadgetDBG_INPUT_MGR, vprDBG_CRITICAL_LVL)
                           << ex.getExtendedDescription() << std::endl
                           << vprDEBUG_FLUSH;
                     }
                     catch (vpr::Exception& ex)
                     {
                        vprDEBUG(gadgetDBG_INPUT_MGR, vprDBG_CRITICAL_LVL)
                           << clrOutBOLD(clrRED, "ERROR")
                           << ": Failed to load cluster plug-in DSO '"
                           << plugin_dso_name << "'\n" << vprDEBUG_FLUSH;
                        vprDEBUG_NEXT(gadgetDBG_INPUT_MGR, vprDBG_CRITICAL_LVL)
                           << "This plug-in will not be usable.\n"
                           << vprDEBUG_FLUSH;
                        vprDEBUG(gadgetDBG_INPUT_MGR, vprDBG_CRITICAL_LVL)
                           << ex.what() << std::endl << vprDEBUG_FLUSH;
                     }
                  }
                  else
                  {
                     vprDEBUG(gadgetDBG_INPUT_MGR, vprDBG_CRITICAL_LVL)
                        << clrOutBOLD(clrRED, "ERROR")
                        << ": Failed to find cluster plug-in DSO '"
                        << plugin_dso_name << "'\n" << vprDEBUG_FLUSH;
                  }
               }
            }

            ret_val = true;
         }

         // Dump the status
         {
            vpr::DebugOutputGuard dbg_output( gadgetDBG_RIM, vprDBG_CONFIG_LVL,
                                    std::string( "New Cluster Manager state:\n" ),
                                    std::string( "-- end state -- \n" ) );
            vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_LVL )
               << (*this) << std::endl << vprDEBUG_FLUSH;
         }
      }

      vpr::Guard<vpr::Mutex> guard( mClusterActiveLock );
      mClusterActive = true;

      return ret_val;         // Return the success flag if we added at all
    }

    /** Remove the pending element from the current configuration.
     *  @pre configCanHandle(element) == true.
     *  @return true iff the element (and any objects it represented)
     *          were successfully removed.
     */
   bool ClusterManager::configRemove( jccl::ConfigElementPtr element )
   {
      if ( recognizeClusterManagerConfig( element ) )
      {
         vprDEBUG( gadgetDBG_RIM,vprDBG_CONFIG_LVL )
            << "[ClusterManager] Shutdown the Cluster: " << element->getName()
            << std::endl << vprDEBUG_FLUSH;
         return( true );
      }
      else
      {
         vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_LVL )
            << "[ClusterManager::configRemove()] ERROR: Something is "
            << "seriously wrong, we should never get here." << std::endl
            << vprDEBUG_FLUSH;
         return( false );
      }
   }

   /** Checks if this handler can process element.
    *  Typically, an implementation of handler will check the element's
    *  description name/token to decide if it knows how to deal with
    *  it.
    *  @return true iff this handler can process element.
    */
   bool ClusterManager::configCanHandle( jccl::ConfigElementPtr element )
   {
      return(recognizeClusterManagerConfig( element ));
   }

   std::string ClusterManager::getElementType()
   {
      return "cluster_manager";
   }

   // ---- Configuration Helper Functions ----
   jccl::ConfigElementPtr ClusterManager::getConfigElementPointer( const std::string& name )
   {
      jccl::ConfigManager* cfg_mgr = jccl::ConfigManager::instance();
      //cfg_mgr->lockPending();
      //cfg_mgr->unlockPending();
      for ( std::list<jccl::ConfigManager::PendingElement>::iterator i = cfg_mgr->getPendingBegin();
            i != cfg_mgr->getPendingEnd() ; ++i )
      {
         if ( (*i).mElement->getName() == name )
         {
            return( (*i).mElement );
         }
      }
      cfg_mgr->lockActive();
      jccl::ConfigElementPtr temp = cfg_mgr->getActiveConfig()->get( name );
      cfg_mgr->unlockActive();
      return( temp );
   }

   /**
    * Dump the current Status of the InputManager, listing all
    * the devices, proxies and internal settings
    */
   std::ostream& operator<<( std::ostream& out, ClusterManager& mgr )
   {
      out << std::endl << "========== ClusterManager Status =========="
          << std::endl;
      out << "preDraw() call count:       " << mgr.mPreDrawCallCount << std::endl;
      out << "postPostFrame() call count: " << mgr.mPostPostFrameCallCount << std::endl;
      out << "Plugins:" << std::endl;

      // Dump Plugins
      for ( std::list<ClusterPlugin*>::iterator i = mgr.mPlugins.begin();
            i != mgr.mPlugins.end();
            ++i )
      {
         if ( (*i) != NULL )
         {
            out << "  name:" << std::setw(30) << std::setfill(' ') << (*i)->getPluginName()
                << "  guid:" << std::setw(30) << std::setfill(' ') << (*i)->getHandlerGUID()
                << "  active:" << std::setw(7) << std::setfill(' ')
                << ( (*i)->isActive() ? "true" : "false" ) << std::endl;
         }
      }

      out << std::endl;

      return out;
   }

} // End of gadget namespace
