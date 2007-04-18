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

package org.vrjuggler.tweek;


/**
 * The main class for the Tweek software system.  This is the class that
 * implements main() and thus is called to run the application.  It holds a
 * reference to the org.vrjuggler.tweek.TweekFrame object that is the full
 * interface to the Plexus controller software.
 *
 * @version $Revision$
 */
public class Tweek
{
   // ========================================================================
   // Public static methods.
   // ========================================================================

   /**
    * Main method.
    */
   public static void main (String[] args)
   {
      try
      {
         TweekCore.instance().init(args);
      }
      catch (Exception e)
      {
         e.printStackTrace();
      }
   }
}
