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
#include <jccl/Config/ConfigElement.h>
#include <gadget/Util/Debug.h>
#include <gadget/Type/StringProxy.h>

namespace gadget
{

std::string StringProxy::getElementType()
{
   return "string_proxy";
}

bool StringProxy::config(jccl::ConfigElementPtr element)
{
vpr::DebugOutputGuard dbg_output(gadgetDBG_INPUT_MGR, vprDBG_STATE_LVL,
                              std::string("----------- configuring STRING PROXY -----------------\n"),
                              std::string("----------- exit: configuring string proxy -----------\n"));

   vprASSERT(element->getID() == getElementType());

   if( ! Proxy::config(element) )
   {
      return false;
   }

   mUnitNum = element->getProperty<int>("unit");
   mDeviceName = element->getProperty<std::string>("device");

   refresh();
   return true;
}

void StringProxy::updateData()
{
   if (!isStupefied())
   {
      // Make sure dependencies are updated.
      getProxiedInputDevice()->updateDataIfNeeded();

      std::string old_state = mData.getString();
      mData = mTypedDevice->getStringData(mUnitNum);
      std::string new_state = mData.getString();
   }
}

} // End of gadget namespace
