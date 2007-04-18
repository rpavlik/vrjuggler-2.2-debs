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

package org.vrjuggler.tweek.event;

import java.util.EventObject;


/**
 * Event type associated with run-time changes in the user "level," a
 * concept that is supposed to indicate the level of expertise of the user.
 *
 * @see UserLevelChangeListener
 *
 * @since 0.0.4
 */
public class UserLevelChangeEvent extends EventObject
{
   public UserLevelChangeEvent (Object source, int old_level, int new_level)
   {
      super(source);
      oldLevel = old_level;
      newLevel = new_level;
   }

   public int getOldLevel ()
   {
      return oldLevel;
   }

   public int getNewLevel ()
   {
      return newLevel;
   }

   private int oldLevel;
   private int newLevel;
}
