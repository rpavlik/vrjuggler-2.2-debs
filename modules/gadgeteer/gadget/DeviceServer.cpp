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
#include <boost/bind.hpp>
#include <gadget/Util/Debug.h>
#include <gadget/Node.h>
#include <gadget/DeviceServer.h>

namespace gadget
{
   DeviceServer::DeviceServer(const std::string& name, gadget::Input* device,
                              const vpr::GUID& plugin_guid)
      : deviceServerTriggerSema(0)
      , deviceServerDoneSema(0)
   {
      vpr::GUID temp;
      temp.generate();

      do
      {
         mId.generate();   // Generate a unique ID for this device
         vprDEBUG(vprDBG_ALL, vprDBG_STATE_LVL)
            << "[DeviceServer] Invalid GUID, generating a new one."
            << std::endl << vprDEBUG_FLUSH;
      }
      while(temp == mId);

      mThreadActive = false;
      mName = name;
      mDevice = device;
      mPluginGUID = plugin_guid;

      mDeviceData = new std::vector<vpr::Uint8>;
      mDataPacket = new cluster::DataPacket(plugin_guid, mId, mDeviceData);
      mBufferObjectWriter = new vpr::BufferObjectWriter(mDeviceData);
   }

   DeviceServer::~DeviceServer()
   {
      delete mDataPacket;
      // mDataPacket will clean up the memory that mDeviceData points
      // to since mDataPacket contains a reference to the ame memory.
      mDeviceData = NULL;
   }

   void DeviceServer::send()
   {
      vpr::Guard<vpr::Mutex> guard(mClientsLock);

      //vprDEBUG(gadgetDBG_RIM,vprDBG_CONFIG_LVL)
      //   << clrOutBOLD(clrMAGENTA,"DeviceServer::send()")
      //   << "Sending Device Data for: " << getName() << std::endl
      //   << vprDEBUG_FLUSH;

      for ( std::vector<gadget::Node*>::iterator i = mClients.begin();
            i != mClients.end();
            ++i )
      {
         //vprDEBUG(gadgetDBG_RIM,vprDBG_CONFIG_LVL)
         //   << "Sending data to: " << (*i)->getName()
         //   << " trying to lock socket" << std::endl << vprDEBUG_FLUSH;

         try
         {
            (*i)->send(mDataPacket);
         }
         catch( cluster::ClusterException cluster_exception )
         {
            vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_LVL )
               << "DeviceServer::send() Caught an exception!"
               << std::endl << vprDEBUG_FLUSH;
            vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_LVL )
               << clrOutBOLD(clrRED, "ERROR:") << cluster_exception.what()
               << std::endl << vprDEBUG_FLUSH;
            vprDEBUG( gadgetDBG_RIM, vprDBG_CONFIG_LVL )
               << "DeviceServer::send() We have lost our connection to: "
               << (*i)->getName() << ":" << (*i)->getPort()
               << std::endl << vprDEBUG_FLUSH;

            (*i)->setStatus( gadget::Node::DISCONNECTED );
            (*i)->shutdown();

            debugDump( vprDBG_CONFIG_LVL );
         }
      }
      //vprDEBUG(gadgetDBG_RIM,vprDBG_CONFIG_LVL)
      //   << clrOutBOLD(clrMAGENTA,"DeviceServer::send()")
      //   << "Done Sending Device Data for: " << getName() << std::endl
      //   << vprDEBUG_FLUSH;
   }

   void DeviceServer::updateLocalData()
   {
      // -BufferObjectWriter
      mBufferObjectWriter->getData()->clear();
      mBufferObjectWriter->setCurPos(0);

      // This updates the mDeviceData which both mBufferedObjectReader and
      // mDevicePacket point to.
      mDevice->writeObject(mBufferObjectWriter);

      // We must update the size of the actual data that we are going to send
      mDataPacket->getHeader()->setPacketLength(
         cluster::Header::RIM_PACKET_HEAD_SIZE
            + 16 /*Plugin GUID*/
            + 16 /*Plugin GUID*/
            + mDeviceData->size()
      );

      // We must serialize the header again so that we can reset the size.
      mDataPacket->getHeader()->serializeHeader();
   }

   void DeviceServer::addClient(gadget::Node* new_client_node)
   {
      vprASSERT(new_client_node != NULL &&
                "You can not add a new client that is NULL");
      vpr::Guard<vpr::Mutex> guard(mClientsLock);

      mClients.push_back(new_client_node);
   }

   void DeviceServer::removeClient(const std::string& host_name)
   {
      vpr::Guard<vpr::Mutex> guard(mClientsLock);

      for (std::vector<gadget::Node*>::iterator i = mClients.begin() ;
            i!= mClients.end() ; i++)
      {
         if ((*i)->getHostname() == host_name)
         {
            mClients.erase(i);
            return;
         }
      }
   }

   void DeviceServer::debugDump(int debugLevel)
   {
      vpr::Guard<vpr::Mutex> guard(mClientsLock);

      vpr::DebugOutputGuard dbg_output(
         gadgetDBG_RIM, debugLevel,
         "-------------- DeviceServer --------------\n",
         "------------------------------------------\n"
      );

      vprDEBUG(gadgetDBG_RIM, debugLevel)
         << "Name:     " << mName << std::endl << vprDEBUG_FLUSH;

      { // Used simply to make the following DebugOutputGuard go out of scope
         vpr::DebugOutputGuard dbg_output2(gadgetDBG_RIM, debugLevel,
                           std::string("------------ Clients ------------\n"),
                           std::string("---------------------------------\n"));
         for ( std::vector<gadget::Node*>::iterator i = mClients.begin();
                i != mClients.end();
                ++i )
         {
            vprDEBUG(gadgetDBG_RIM, debugLevel)
               << "-------- " << (*i)->getName() << " --------" << std::endl
               << vprDEBUG_FLUSH;
            vprDEBUG(gadgetDBG_RIM, debugLevel)
               << "       Hostname: " << (*i)->getHostname() << std::endl
               << vprDEBUG_FLUSH;
            vprDEBUG(gadgetDBG_RIM, debugLevel)
               << "----------------------------------" << std::endl
               << vprDEBUG_FLUSH;
         }
      }
   }
} // End of gadget namespace
