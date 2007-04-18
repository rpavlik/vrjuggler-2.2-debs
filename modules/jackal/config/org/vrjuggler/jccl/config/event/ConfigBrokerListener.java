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
package org.vrjuggler.jccl.config.event;

import java.util.EventListener;

/**
 * Interface for an observer to receive notifications of changes to a config
 * broker.
 *
 * @see org.vrjuggler.jccl.config.ConfigBroker
 */
public interface ConfigBrokerListener
   extends EventListener
{
   /**
    * Gives notification that a resource was added to the config broker.
    *
    * @param evt     the config broker event
    */
   public void resourceAdded(ConfigBrokerEvent evt);

   /**
    * Gives notification that a resource was removed from the config broker.
    *
    * @param evt     the config broker event
    */
   public void resourceRemoved(ConfigBrokerEvent evt);
}
