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

#ifndef _VPR_ERROR_IMPL_NSPR_H_
#define _VPR_ERROR_IMPL_NSPR_H_

#include <vpr/vprConfig.h>
#include <iostream>
#include <prerror.h>
#include <vpr/Util/ErrorBase.h>


namespace vpr
{

/** \class ErrorImplNSPR ErrorImplNSPR.h vpr/Util/Error.h
 *
 * NSPR Implementation of a cross-platform error reporting class.
 */
class VPR_CLASS_API ErrorImplNSPR : public ErrorBase
{
public:
   /** @since 1.1.6 */
   static std::string getCurrentErrorMsg();

   /** Output the current error message to a stream. */
   static void outputCurrentError(std::ostream& out, const std::string& prefix);

   static ErrorType getCurrentError()
   {
      // TODO: Implement error conversion to get the current error.
      return NoError;
   }

protected:
   /*
   static PRErrorCode convertErrorVprToNspr(ErrorType mask)
   {
      return PR_MAX_ERROR;
   }

   static ErrorType convertErrorNsprToVpr(PRErrorCode mask)
   {
      return NoError;
   }
   */
};

} // End of vpr namespace


#endif  /* _VPR_ERROR_IMP_NSPR_H_ */
