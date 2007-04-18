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

#ifndef _VRJ_FRUSTUM_H_
#define _VRJ_FRUSTUM_H_

#include <vrj/vrjConfig.h>

#include <iostream>


namespace vrj
{

/** \class Frustum Frustum.h vrj/Display/Frustum.h
 *
 * A frustum object.  This is a frustum.
 *
 * @date 10-1-97
 */
class VJ_CLASS_API Frustum
{
public:
   enum entry
   {
      VJ_LEFT = 0,
      VJ_RIGHT = 1,
      VJ_BOTTOM,
      VJ_TOP,
      VJ_NEAR,
      VJ_FAR
   };

   Frustum()
   {
      frust[0] = frust[1] = frust[2] = 0.0f;
      frust[3] = frust[4] = frust[5] = 0.0f;
   }

   void setBottomLeftTopRight(const float bottomVal, const float leftVal,
                              const float topVal, const float rightVal)
   {
      frust[VJ_BOTTOM] = bottomVal;
      frust[VJ_LEFT]   = leftVal;
      frust[VJ_TOP]    = topVal;
      frust[VJ_RIGHT]  = rightVal;
   }

   void setNearFar(const float nearVal, const float farVal)
   {
      frust[VJ_NEAR] = nearVal;
      frust[VJ_FAR]  = farVal;
   }

   void set(const float leftVal, const float rightVal, const float bottomVal,
            const float topVal, const float nearVal, const float farVal)
   {
      frust[VJ_LEFT]   = leftVal;
      frust[VJ_RIGHT]  = rightVal;
      frust[VJ_BOTTOM] = bottomVal;
      frust[VJ_TOP]    = topVal;
      frust[VJ_NEAR]   = nearVal;
      frust[VJ_FAR]    = farVal;
   }

   float& operator[](int elt)
   {
      return frust[elt];
   }

public:
   float frust[6];  /**< Left, Right, Bottom, Top, Near, Far */
};

// ---- FRIEND FUNCTIONS ---- //
std::ostream& operator<<(std::ostream& out, Frustum& _frust);

}

#endif
