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

/**
 * \file
 *
 * @note This file MUST be included by vpr/Thread/Thread.h, not the other way
 *       around.
 */

#ifndef _VPR_THREAD_KEY_SGI_H_
#define _VPR_THREAD_KEY_SGI_H_
//#pragma once

#include <vpr/vprConfig.h>
#include <sys/types.h>


namespace vpr
{

// Key destructor function type.
typedef void (*KeyDestructor)(void*);

/** \class ThreadKeySGI ThreadKeySGI.h vpr/Threaed/Thread.h
 *
 * Wrapper around SPROC thread-specific data.  It is typedef'd to
 * vpr::ThreadKey.
 */
class ThreadKeySGI
{
public:
   /**
    * Creates a key that knows how to delete itself using a function pointer.
    *
    * @post A key is created and is associated with the specified destructor
    *       function and argument.
    *
    * @param destructor The destructor function for the key.
    */
   ThreadKeySGI(KeyDestructor destructor = NULL)
   {
      keycreate(destructor);
   }

   /**
    * Destructor.
    */
   ~ThreadKeySGI()
   {
      keyfree();
   }

   /**
    * Allocates a keyp that is used to identify data that is specific to
    * each thread in the process, is global to all threads in the process
    * and is destroyed using the spcefied destructor function.
    *
    * @post A key is created and is associated with the specified
    *        destructor function and argument.
    *
    * @param destructor  The destructor function for the key.  This uses
    *                    the functor data structure.
    *
    * @return 0 is returned on successful complection
    * @return -1 is returned to indicate an error.
    */
   int keycreate(KeyDestructor destructor = NULL)
   {
      std::cerr << "vpr::ThreadKeySGI::keycreate() not implemented yet!\n";

      return -1;
   }

   /**
    * Frees up the key so that other threads may reuse it.
    *
    * @pre The specified key must have been properly created using the
    *       keycreate() member function.
    * @post The specified key is destroyed using the destructor function
    *        previously associated with it, and its resources are freed.
    *
    * @return  0 is returned upon successful completion.
    * @return -1 is returned to indicate an error.
    */
   int keyfree()
   {
      std::cerr << "vpr::ThreadKeySGI::keyfree() not implemented yet!\n";

      return -1;
   }

   /**
    * Binds value to the thread-specific data key for the calling thread.
    *
    * @pre The specified key must have been properly created using the
    *      keycreate() member function.
    * @post The specified value is associated with the key for the calling
    *        thread.
    *
    * @param value Address containing data to be associated with the
    *              specified key for the current thread.
    *
    * @return  0 is returned upon successful completion.
    * @return -1 is returned to indicate an error.
    */
   int setspecific(void* value)
   {
      std::cerr << "vpr::ThreadKeySGI::setspecific() not implemented yet!\n";

      return -1;
   }

   /**
    * Stores the current value bound to key for the calling thread into
    * the location pointed to by valuep.
    *
    * @pre The specified key must have been properly created using the
    *       keycreate() member function.
    * @post The value associated with the key is obtained and stored in the
    *        pointer \p valuep so that the caller may work with it.
    *
    * @param valuep  Address of the current data value associated with the
    *                 key.
    *
    * @return  0 is returned upon successful completion.
    * @return -1 is returned to indicate an error.
    */
   int getspecific(void** valuep)
   {
      std::cerr << "vpr::ThreadKeySGI::getspecific() not implemented yet!\n";

      return -1;
   }

private:
   int keyID;      /**<  Thread key ID */
};

} // End of vpr namespace


#endif  /* _VPR_THREAD_KEY_SGI_H_ */
