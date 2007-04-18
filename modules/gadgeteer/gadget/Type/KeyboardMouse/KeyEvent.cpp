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

#include <vpr/IO/ObjectWriter.h>
#include <vpr/IO/ObjectReader.h>
#include <gadget/Type/KeyboardMouse/KeyEvent.h>


namespace gadget
{

KeyEvent::KeyEvent(const gadget::EventType type, const gadget::Keys key,
                   const int mask, const unsigned long time,
                   const char asciiKey, const wchar_t unicodeKey)
   : gadget::Event(type, time)
   , mKey(key)
   , mModifierMask(mask)
   , mAsciiKey(asciiKey)
   , mUnicodeKey(unicodeKey)
{
   /* Do nothing. */ ;
}

KeyEvent::KeyEvent() 
   : gadget::Event(NoEvent, 0)
   , mKey(gadget::KEY_SPACE)
   , mModifierMask(0)
   , mAsciiKey(' ')
   , mUnicodeKey(' ')
{
   /* Do nothing. */ ;
}

// Serializes this event using the given ObjectWriter.
void KeyEvent::writeObject(vpr::ObjectWriter* writer)
{
   writer->writeUint16(mType);

   // Serialize all member variables
   writer->writeUint32(mKey);
   writer->writeUint32(mModifierMask);
   writer->writeUint64(mTime);
   writer->writeUint8(mAsciiKey);
}

// De-serializes this event using the given ObjectReader.
void KeyEvent::readObject(vpr::ObjectReader* reader)
{
   // We have already read the type in KeyboardMouse to decide
   // if we should construct a KeyEvent or a MouseEvent
   //mType = reader->readUint16();

   // De-Serialize all member variables
   mKey = (gadget::Keys)reader->readUint32();
   mModifierMask = reader->readUint32();
   mTime = reader->readUint64();
   mAsciiKey = reader->readUint8();
}

} // End of gadget namespace
