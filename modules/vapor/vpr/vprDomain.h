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

#ifndef _VPR_DOMAIN_H_
#define _VPR_DOMAIN_H_

// The following define "domains" supported by VPR.  These are used to
// determine which platform specific headers should be included when
// compiling.  See vpr/Thread/Thread.h for an example of these values are
// used with the domain settings defined below.
#define VPR_DOMAIN_NONE       0
#define VPR_DOMAIN_NSPR       1
#define VPR_DOMAIN_POSIX      2
#define VPR_DOMAIN_IRIX_SPROC 3
#define VPR_DOMAIN_SIMULATOR  4
#define VPR_DOMAIN_WIN32      5

// This is the VPR I/O domain.  It defines all the platform-independent types
// used for doing I/O (sockets and serial ports mostly) in VPR.
namespace vpr
{
   template<class> class Selector_t;
   template<class> class SerialPort_t;

   template<class BASE_ONE, class BASE_TWO> class IOStatsStrategyAdapter;

#ifdef VPR_SIMULATOR
#  define VPR_IO_DOMAIN_INCLUDE VPR_DOMAIN_SIMULATOR

   /** \struct SocketConfiguration vprDomain.h vpr/vprDomain.h
    *
    * Socket configuration for simulator sockets.
    */
   struct SocketConfiguration
   {
      typedef class SocketImplSIM           SocketImpl;
      typedef class SocketDatagramImplSIM   SocketDatagramImpl;
      typedef class SocketStreamImplSIM     SocketStreamImpl;
      typedef class NullIOStatsStrategy     SocketIOStatsStrategy;
   };

   typedef class InetAddrSIM InetAddr;
   typedef class IOSysSIM    IOSys;

   typedef class Selector_t<class SelectorImplSIM> Selector;

   /* Don't allow serial ports in simulator for now.
   * This is because IOSys is of the wrong type and there is
   * not an easy fix for this right now.
   */

//#  ifdef VPR_OS_Windows
//      typedef SerialPort_t<class SerialPortImplWin32>   SerialPort;
//#  else
//      typedef SerialPort_t<class SerialPortImplTermios> SerialPort;
//#  endif  /* VPR_OS_Windows */

#else /* ! ifdef VPR_SIMULATOR */

#ifdef VPR_USE_NSPR
#  define VPR_IO_DOMAIN_INCLUDE VPR_DOMAIN_NSPR

   /**
    * Socket configuration for NSPR sockets.
    */
   struct SocketConfiguration
   {
      typedef class SocketImplNSPR           SocketImpl;
      typedef class SocketDatagramImplNSPR   SocketDatagramImpl;
      typedef class SocketStreamImplNSPR     SocketStreamImpl;
      typedef class NullIOStatsStrategy      SocketIOStatsStrategy;
   };

   typedef class InetAddrNSPR InetAddr;
   typedef class IOSysNSPR    IOSys;

#ifdef __SUNPRO_CC
   class Selector_t<class T>;
   class SelectorImplNSPR;

   typedef Selector_t<SelectorImplNSPR> Selector;
#else
   typedef class Selector_t<class SelectorImplNSPR> Selector;
#endif

#  ifdef VPR_OS_Windows
      typedef SerialPort_t<class SerialPortImplWin32>   SerialPort;
#  else
      typedef SerialPort_t<class SerialPortImplTermios> SerialPort;
#  endif  /* VPR_OS_Windows */

/* POSIX and SPROC */
#else
#  define VPR_IO_DOMAIN_INCLUDE VPR_DOMAIN_POSIX

   /**
    * Socket configuration for BSD sockets.
    */
   struct SocketConfiguration
   {
      typedef class SocketImplBSD           SocketImpl;
      typedef class SocketDatagramImplBSD   SocketDatagramImpl;
      typedef class SocketStreamImplBSD     SocketStreamImpl;
      typedef class NullIOStatsStrategy     SocketIOStatsStrategy;
   };

   typedef class InetAddrBSD    InetAddr;
   typedef class IOSysUnix      IOSys;

   typedef class Selector_t<class SelectorImplBSD>         Selector;
   typedef class SerialPort_t<class SerialPortImplTermios> SerialPort;

#endif /* ifdef VPR_USE_NSPR */
#endif /* ifdef VPR_SIMULATOR */

}

// This is the threading (and synchronization) domain.  It defins the types
// used for platform-independent constructs for doing multi-threaded
// programming.
namespace vpr
{
#if defined(VPR_USE_WINTHREADS)
#  define VPR_THREAD_DOMAIN_INCLUDE VPR_DOMAIN_WIN32

   typedef class ErrorImplWin32 Error;
   typedef class SystemNSPR     System;
   typedef class CondVarWin32   CondVar;
   typedef class MutexWin32     Mutex;
   typedef class SemaphoreWin32 Semaphore;
   typedef class ThreadWin32    Thread;
   typedef class ThreadKeyWin32 KeyId;

#elif defined(VPR_USE_NSPR)
#  define VPR_THREAD_DOMAIN_INCLUDE VPR_DOMAIN_NSPR

   typedef class ErrorImplNSPR Error;
   typedef class SystemNSPR    System;
   typedef class CondVarNSPR   CondVar;
   typedef class MutexNSPR     Mutex;
   typedef class RWMutexNSPR   RWMutex;
   typedef class SemaphoreNSPR Semaphore;
   typedef class ThreadNSPR    Thread;
   typedef class ThreadKeyNSPR KeyId;

/* POSIX and SPROC */
#else
   typedef class ErrorImplPosix Error;
   typedef class SystemPosix    System;

#  ifdef VPR_USE_IRIX_SPROC
#     define VPR_THREAD_DOMAIN_INCLUDE VPR_DOMAIN_IRIX_SPROC

      typedef class BarrierSGI     Barrier;
      typedef class CondVarGeneric CondVar;
      typedef class MutexSGI       Mutex;
      typedef class SemaphoreSGI   Semaphore;

      typedef struct cancel_state  cancel_state_t;
      typedef class ThreadSGI      Thread;
      typedef class ThreadKeySGI   KeyId;
#  else
#     define VPR_THREAD_DOMAIN_INCLUDE VPR_DOMAIN_POSIX

      typedef class CondVarPosix   CondVar;
      typedef class MutexPosix     Mutex;
      typedef class RWMutexPosix   RWMutex;
      typedef class SemaphorePosix Semaphore;

      typedef int                  cancel_state_t;
      typedef class ThreadPosix    Thread;
      typedef class ThreadKeyPosix KeyId;
#  endif /* ifdef VPR_USE_IRIX_SPROC */

#endif /* ifdef VPR_USE_NSPR */
}

#ifndef VPR_IO_DOMAIN_INCLUDE
#define VPR_IO_DOMAIN_INCLUDE VPR_DOMAIN_NONE
#endif

#ifndef VPR_THREAD_DOMAIN_INCLUDE
#define VPR_THREAD_DOMAIN_INCLUDE VPR_DOMAIN_NONE
#endif

#endif  /* ifndef _VPR_DOMAIN_H_ */
