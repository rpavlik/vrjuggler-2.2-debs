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

#include <gadget/Util/Debug.h>

#include <cluster/Packets/DeviceAck.h>
#include <cluster/Packets/PacketFactory.h>

namespace cluster
{
   CLUSTER_REGISTER_CLUSTER_PACKET_CREATOR(DeviceAck);

   DeviceAck::DeviceAck(const vpr::GUID& plugin_id, const vpr::GUID& id, 
                        const std::string& device_name, 
                        const std::string& device_base_type, bool ack)
   {
      // Set the local member variables using the given values.
      mPluginId = plugin_id;
      mId = id;
      mDeviceName = device_name;
      mDeviceBaseType = device_base_type;
      mAck = ack;
      
      // Get the localhost name.
      mHostname = vpr::InetAddr::getLocalHost().getHostname();

      // Create a Header for this packet with the correect type and size.
      mHeader = new Header(Header::RIM_PACKET,
                                      Header::RIM_DEVICE_ACK,
                                      Header::RIM_PACKET_HEAD_SIZE 
                                      + 16 /*mPluginId*/
                                      + 16 /*mId*/
                                      + vpr::BufferObjectReader::STRING_LENGTH_SIZE
                                      + mDeviceName.size() /*length of mDeviceName*/
                                      + vpr::BufferObjectReader::STRING_LENGTH_SIZE
                                      + mDeviceBaseType.size() /*length of mDeviceBaseType*/
                                      + vpr::BufferObjectReader::STRING_LENGTH_SIZE
                                      + mHostname.size() /*length of mDeviceBaseType*/
                                      + 1 /*mAck*/,
                                      0/*Field not curently used*/);                      
      // Serialize the given data.
      serialize();
   }

   void DeviceAck::serialize()
   {
      // Clear the data stream.
      mPacketWriter->getData()->clear();
      mPacketWriter->setCurPos(0);

      // Serialize the header.
      mHeader->serializeHeader();
      
      // Serialize plugin GUID
      mPluginId.writeObject(mPacketWriter);
      
      // Serialize Device GUID
      mId.writeObject(mPacketWriter);
      
      // Serialize the Device Name
      mPacketWriter->writeString(mDeviceName);
      
      // Serialize the Base Type of the acknowledged device
      mPacketWriter->writeString(mDeviceBaseType);

      // Serialize the hostname of the acknowledging node
      mPacketWriter->writeString(mHostname);

      // Serialize the Ack boolean
      mPacketWriter->writeBool(mAck);
   }

   void DeviceAck::parse(vpr::BufferObjectReader* reader)
   {
      // De-Serialize plugin GUID
      mPluginId.readObject(reader);

      // De-Serialize Device GUID
      mId.readObject(reader);
         
      // De-Serialize the Device Name
      mDeviceName = reader->readString();

      // De-Serialize the Base Type of the acknowledged device
      mDeviceBaseType = reader->readString();

      // De-Serialize the hostname of the acknowledging node
      mHostname = reader->readString();

      // De-Serialize the Ack boolean
      mAck = reader->readBool();
   }

   void DeviceAck::printData(int debug_level)
   {
      vprDEBUG_BEGIN(gadgetDBG_RIM,debug_level) 
         <<  clrOutBOLD(clrYELLOW,"==== Device Ack Packet Data ====\n") << vprDEBUG_FLUSH;
      
      Packet::printData(debug_level);

      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Plugin GUID:      ") << mPluginId.toString()
         << std::endl << vprDEBUG_FLUSH;
      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Device ID:        ") << mId.toString()
         << std::endl << vprDEBUG_FLUSH;
      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Device Name:      ") << mDeviceName
         << std::endl << vprDEBUG_FLUSH;
      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Device Base Type: ") << mDeviceBaseType 
         << std::endl << vprDEBUG_FLUSH;
      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Remote Hostname:  ") << mHostname
         << std::endl << vprDEBUG_FLUSH;
      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Ack or Nack:      ") << (mAck ? "Ack" : "Nack")  << std::endl
         << std::endl << vprDEBUG_FLUSH;

      vprDEBUG_END(gadgetDBG_RIM,debug_level) 
         <<  clrOutBOLD(clrYELLOW,"================================\n") << vprDEBUG_FLUSH;
   }
}   // end namespace gadget
