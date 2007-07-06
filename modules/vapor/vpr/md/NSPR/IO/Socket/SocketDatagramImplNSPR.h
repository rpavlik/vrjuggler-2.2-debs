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

#ifndef _VPR_SOCKET_DATAGRAM_IMPL_NSPR_H_
#define _VPR_SOCKET_DATAGRAM_IMPL_NSPR_H_

#include <vpr/vprConfig.h>

#include <vpr/md/NSPR/IO/Socket/SocketImplNSPR.h>


namespace vpr
{

/** \class SocketDatagramImplNSPR SocketDatagramImplNSPR.h vpr/IO/Socket/SocketDatagram.h
 *
 * NSPR implementation of the datagram socket interface.  This is used in
 * conjunction with vpr::SocketConfiguration to create the typedef
 * vpr::SocketDatagram.
 */
class VPR_CLASS_API SocketDatagramImplNSPR : public SocketImplNSPR
{
public:
   typedef SocketImplNSPR Parent;

   // ========================================================================
   // vpr::SocketDatagram implementation.
   // ========================================================================

   /**
    * Default constructor.
    *
    * @post The member variables are initialized to default values.  The
    *       socket type is set to vpr::SocketTypes::DATAGRAM.
    */
   SocketDatagramImplNSPR();

   /**
    * Constructor.
    *
    * @post The member variables are initialized to default values.  The
    *       socket type is set to vpr::SocketTypes::DATAGRAM.
    *
    * @param localAddr  The local address to which this socket will be bound.
    * @param remoteAddr The remote address whith which this socket will
    *                   communicate.
    */
   SocketDatagramImplNSPR(const InetAddr& localAddr,
                          const InetAddr& remoteAddr);

   /** Copy constructor. */
   SocketDatagramImplNSPR(const SocketDatagramImplNSPR& sock);

   /**
    * Receives a message from the specified address.
    *
    * @return The number of bytes read into the buffer is returned.
    *
    * @throws vpr::SocketException     If the socket is not connected.
    * @throws vpr::WouldBlockException If the file is in non-blocking mode,
    *                                  and there is no data to read.
    * @throws vpr::TimeoutException    If the read could not begin within the
    *                                  timeout interval.
    * @throws vpr::IOException         If the read operation failed.
    */
   vpr::Uint32 recvfrom(void* msg, const vpr::Uint32 length,
                        vpr::InetAddr& from,
                        const vpr::Interval timeout = vpr::Interval::NoTimeout);

   /**
    * Sends a message to the specified address.
    *
    * @return The number of bytes written to the socket is returned.
    */
   vpr::Uint32 sendto(const void* msg, const vpr::Uint32 length,
                      const vpr::InetAddr& to,
                      const vpr::Interval timeout = vpr::Interval::NoTimeout);
};

} // End of namespace


#endif  /* _VPR_SOCKET_DATAGRAM_IMPL_NSPR_H_ */
