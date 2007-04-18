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

#ifndef _CLUSTER_RIM_PLUGIN_H
#define _CLUSTER_RIM_PLUGIN_H

#include <cluster/PluginConfig.h>

#include <list>

#include <vpr/Util/Singleton.h>
#include <vpr/Sync/Mutex.h>
#include <vpr/Util/Assert.h>

#include <cluster/ClusterPlugin.h>

#include <gadget/RemoteInputManager.h>

#include <jccl/Config/ConfigElementPtr.h>

namespace gadget
{
   class Node;
   class Input;
}
namespace cluster
{
   class VirtualDevice;
   class DeviceServer;
   class DeviceRequest;
   class Packet;

class GADGET_CLUSTER_PLUGIN_CLASS_API RIMPlugin : public cluster::ClusterPlugin
{
public:
   RIMPlugin();
   virtual ~RIMPlugin();

   vpr::GUID getHandlerGUID()
   {
      return mHandlerGUID;
   }
   
   /**
    * This function is called when the network losses
    * a connection to a Node. The RIMPlugin needs to
    * handle this by removing all VirtualDevices on 
    * the lost node. And removing the node as a client
    * from all DeviceServers.
    */
   void recoverFromLostNode(gadget::Node* lost_node);
   
   /**
    * Handle a incoming packet.
    */
   void handlePacket(Packet* packet, gadget::Node* node);

   /** Calls any action needed by this plugin before draw()
    *
    *  This function was inherited from the
    *  ClusterPlugin abstract class. 
    */
   virtual void preDraw();

   virtual void sendRequests();
   
   /** Calls any action needed by this plugin before postFrame()
    *
    *  This function was inherited from the
    *  ClusterPlugin abstract class. 
    */
   virtual void postPostFrame();
   
   /** Returns the status of RIMPlugin
    *
    *  @return true If plugin is completly configured
    *               and has no pending tasks.
    *  
    */
   virtual bool isPluginReady();
   
   /** 
    *  Returns the name of the this plugin
    */
   virtual std::string getPluginName()
   {
      return(std::string("RIMPlugin"));
   }
   
   virtual std::string getHandlerName()
   {
      return(std::string("RIMPlugin"));
   }

     
   //////////////////////////
   //    CONFIG METHODS    //
   //////////////////////////

   bool configAdd(jccl::ConfigElementPtr element);
   bool configRemove(jccl::ConfigElementPtr element);
   bool configCanHandle(jccl::ConfigElementPtr element);
   jccl::ConfigElementPtr getConfigElementPointer(std::string& name);
   
private:   
   vpr::GUID                    mHandlerGUID;
   gadget::RemoteInputManager   mRIM;
};

} // end namespace gadget

#endif /* _CLUSTER_RIM_PLUGIN_H */
