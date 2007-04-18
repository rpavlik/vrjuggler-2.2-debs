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

#include <cluster/Packets/PacketFactory.h>
#include <cluster/Packets/EndBlock.h>
#include <boost/concept_check.hpp>

namespace cluster
{
CLUSTER_REGISTER_CLUSTER_PACKET_CREATOR(EndBlock);

   /**
    * Create a EndBlock packet to signal that the local node has reached the
    * end of the frame.
    *
    * @param frame_number The current number of frames that have been drawn.
    */
   EndBlock::EndBlock(const vpr::Uint32& frame_number)
   {
      // Set the local member variables using the given values.
      mTempVar = frame_number;
      // Create a Header for this packet with the correect type and size.
      mHeader = new Header(Header::RIM_PACKET,
                                       Header::RIM_END_BLOCK,
                                       Header::RIM_PACKET_HEAD_SIZE
                                       + 2 /*Temp Variable*/,
                                       frame_number);
      // Serialize the given data.
      serialize();
   }

   /**
    * Serializes member variables into a data stream.
    */
   void EndBlock::serialize()
   {
      //mTempVar = Header::RIM_END_BLOCK;
      // Clear the data stream.
      mPacketWriter->getData()->clear();
      mPacketWriter->setCurPos(0);

      // Serialize the header.
      mHeader->serializeHeader();      
      
      // Serialize the Temp Var
      mPacketWriter->writeUint16(mTempVar);
   }

   /**
    * Parses the data stream into the local member variables.
    */
   void EndBlock::parse(vpr::BufferObjectReader* reader)
   {
      // De-Serialize the Temp Var
      mTempVar = reader->readUint16();
   }

   /**
    * Print the data to the screen in a readable form.
    */
   void EndBlock::printData(int debug_level)
   {
      boost::ignore_unused_variable_warning(debug_level);
/*      vprDEBUG_BEGIN(gadgetDBG_RIM,vprDBG_CONFIG_LVL)
         <<  clrOutBOLD(clrYELLOW,"====== END BLOCK ======\n") << vprDEBUG_FLUSH;

      Packet::printData(debug_level);

      vprDEBUG(gadgetDBG_RIM,debug_level)
         << clrOutBOLD(clrYELLOW, "New State:    ") << mNewState
         << std::endl << vprDEBUG_FLUSH;

      vprDEBUG_END(gadgetDBG_RIM,vprDBG_CONFIG_LVL)
         <<  clrOutBOLD(clrYELLOW,"=======================\n") << vprDEBUG_FLUSH;
*/
   }
} // end namespace cluster




