/****************** <VPR heading BEGIN do not edit this line> *****************
 *
 * VR Juggler Portable Runtime
 *
 * Original Authors:
 *   Allen Bierbaum, Patrick Hartling, Kevin Meinert, Carolina Cruz-Neira
 *
 ****************** <VPR heading END do not edit this line> ******************/

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

#include <vpr/vprConfig.h>
#include <vpr/IO/Selector.h>
#include <vpr/Util/Assert.h>
#include <vpr/Util/Debug.h>
#include <vpr/IO/Socket/SocketConnector.h>


namespace vpr
{

void SocketConnector::connect(vpr::SocketStream& newStream,
                              const vpr::InetAddr& remoteAddr,
                              const vpr::Interval& timeout,
                              const vpr::InetAddr& localAddr)
{
   //vpr::InetAddr remote_addr;

   // Open the socket
   checkOpen(newStream);

   // Start the connection
   connectStart(newStream, timeout, localAddr);

   newStream.setRemoteAddr(remoteAddr);

   // Attempt the connection
   newStream.connect(timeout);
}

// Complete a non-blocking connection
// Try to complete a non-blocking connection.
void SocketConnector::complete(vpr::SocketStream& newStream,
                               const vpr::Interval& timeout)
{
   if ( newStream.isConnected() )
   {
      // XXX: Should this actually be a failure
      return;
   }

   // If non-blocking, then we can only wait as long as the timeout
   if ( ! newStream.isBlocking() )
   {
      vpr::IOSys::Handle handle;
      vpr::Selector selector;
      vpr::Uint16 num_events;

      // Use the selector to be informed when the SocketStream object is ready
      // to be used.  That is, when the object is connected.
      handle = newStream.getHandle();
      selector.addHandle(handle);
      selector.setIn(handle, vpr::Selector::Read | vpr::Selector::Write);
      selector.select(num_events, timeout);

      // If the selector told us that our handle is ready, we are successfully
      // connected.
      if ( selector.getOut(handle) & (vpr::Selector::Read | vpr::Selector::Write) )
      {
         /*
         if ( remoteAddr != NULL )
         {
            (*remoteAddr) = newStream.getRemoteAddr();
         }
         */
      }
      // else Use the status from the selector
   }
   else     // Not a non-blocking socket
   {
      vprASSERT(false && "Should not call complete on a non-blocking socket");
      /*
      if ( remoteAddr != NULL )
      {
         (*remoteAddr) = newStream.getRemoteAddr();
      }
      */
   }
}

void SocketConnector::checkOpen(SocketStream& newStream)
{
   if ( ! newStream.isOpen() )
   {
      try
      {
         newStream.open();
      }
      catch (IOException& ex)
      {
         vprDEBUG(vprDBG_ALL, vprDBG_CRITICAL_LVL)
            << "vpr::Connector:CheckOpen: Failed to open socket\n"
            << vprDEBUG_FLUSH;
         throw;
      }
   }
}

// Do preconnection rituals
// - If not bound, then bind to local addr
// - If timeout == 0, then set nonblocking
void SocketConnector::connectStart(SocketStream& newStream,
                                   const vpr::Interval& timeout,
                                   const vpr::InetAddr& localAddr)
{
   vprASSERT(newStream.isOpen());

   if ( ! newStream.isBound() )      // If we are not bound yet
   {
      // If timeout is 0, then we are non-blocking
      if ( vpr::Interval::NoWait == timeout )
      {
         newStream.setBlocking(false);
      }

      // Set addr and bind
      newStream.setLocalAddr(localAddr);
      newStream.bind();
   }
}

}
