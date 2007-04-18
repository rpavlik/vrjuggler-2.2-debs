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
#include <cluster/Packets/ConnectionRequest.h>
#include <cluster/Packets/PacketFactory.h>

namespace cluster
{
   CLUSTER_REGISTER_CLUSTER_PACKET_CREATOR(ConnectionRequest);

   ConnectionRequest::ConnectionRequest(std::string host_name, vpr::Uint16 port)
   {
      // Set the local member variables using the given values.
      mHostname = host_name;
      mPort = port;
            
      // Create a Header for this packet with the correect type and size.
      mHeader = new Header(Header::RIM_PACKET,
                                      Header::RIM_CONNECTION_REQ,
                                      Header::RIM_PACKET_HEAD_SIZE 
                                      + vpr::BufferObjectReader::STRING_LENGTH_SIZE
                                      + mHostname.size()
                                      + 2 /*mPort*/
                                      ,0/*Field not curently used*/);
      // Serialize the given data.
      serialize();
   }

   void ConnectionRequest::serialize()
   {
      // Clear the data stream.
      mPacketWriter->getData()->clear();
      mPacketWriter->setCurPos(0);

      // Serialize the header.
      mHeader->serializeHeader();
      
      // Serialize the hostname of the requesting machine.
      mPacketWriter->writeString(mHostname);
               
      // Serialize the listening port of the requesting machine
      mPacketWriter->writeUint16(mPort);                  
   }
   
   void ConnectionRequest::parse(vpr::BufferObjectReader* reader)
   {
      // De-Serialize the hostname of the requesting machine.
      mHostname = reader->readString();
         
      // De-Serialize the listening port of the requesting machine
      mPort = reader->readUint16();
   }

   void ConnectionRequest::printData(int debug_level)
   {
      vprDEBUG_BEGIN(gadgetDBG_RIM,debug_level) 
         <<  clrOutBOLD(clrYELLOW,"==== Connection Request Packet Data ====\n") << vprDEBUG_FLUSH;
      
      Packet::printData(debug_level);

      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Host Name:    ") << mHostname
         << std::endl << vprDEBUG_FLUSH;
      vprDEBUG(gadgetDBG_RIM,debug_level) 
         << clrOutBOLD(clrYELLOW, "Port:         ") << mPort
         << std::endl << vprDEBUG_FLUSH;

      vprDEBUG_END(gadgetDBG_RIM,debug_level) 
         <<  clrOutBOLD(clrYELLOW,"========================================\n") << vprDEBUG_FLUSH;
   }
}   // end namespace gadget
