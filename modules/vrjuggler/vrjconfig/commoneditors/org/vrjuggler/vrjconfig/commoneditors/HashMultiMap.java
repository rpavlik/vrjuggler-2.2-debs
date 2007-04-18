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

package org.vrjuggler.vrjconfig.commoneditors;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;


public class HashMultiMap
   extends HashMap
{
   public Object get(Object key, int index)
   {
      Object value = super.get(key);

      if ( value == null )
      {
         return null;
      }
      else
      {
         List list = (List) value;
         return list.get(index);
      }
   }

   public Object put(Object key, Object value)
   {
      Object orig_list = super.get(key);
      Object new_list  = orig_list;

      if ( new_list == null )
      {
         new_list = new ArrayList();
         super.put(key, new_list);
      }

      List list = (List) new_list;
      list.add(value);

      return orig_list;
   }
}
