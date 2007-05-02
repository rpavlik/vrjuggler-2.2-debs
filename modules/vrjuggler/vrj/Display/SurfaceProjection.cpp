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

#include <vrj/vrjConfig.h>

#include <iomanip>
//#include <sys/types.h>
#include <string>

#include <gmtl/Vec.h>
#include <gmtl/Matrix.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>
#include <gmtl/Output.h>
#include <gmtl/Xforms.h>

#include <jccl/Config/ConfigElement.h>
#include <vrj/Kernel/Kernel.h>
#include <vrj/Util/Debug.h>
#include <vrj/Display/DisplayExceptions.h>
#include <vrj/Display/SurfaceProjection.h>


namespace vrj
{

SurfaceProjection::SurfaceProjection(const gmtl::Point3f& llCorner,
                                     const gmtl::Point3f& lrCorner,
                                     const gmtl::Point3f& urCorner,
                                     const gmtl::Point3f& ulCorner)
   : mLLCorner(llCorner)
   , mLRCorner(lrCorner)
   , mURCorner(urCorner)
   , mULCorner(ulCorner)
{
   /* Do nothing. */ ;
}

void SurfaceProjection::validateCorners()
{
   gmtl::Vec3f norm1, norm2;
   gmtl::Vec3f bot_side = mLRCorner-mLLCorner;
   gmtl::Vec3f diag = mULCorner-mLRCorner;
   gmtl::Vec3f right_side = mURCorner-mLRCorner;
   gmtl::cross(norm1, bot_side, diag);
   gmtl::cross(norm2, bot_side, right_side);
   gmtl::normalize( norm1 ); gmtl::normalize(norm2);

   if ( ! gmtl::isEqual(norm1, norm2, 1e-4f) )
   {
      std::ostringstream msg_stream;
      msg_stream << "The corners " << mLLCorner << ", " << mLRCorner << ", "
                 << mURCorner << ", " << mULCorner
                 << " do not form a valid surface";
      throw InvalidSurfaceException(msg_stream.str());
   }
}

// Just call the base class constructor
void SurfaceProjection::config(jccl::ConfigElementPtr element)
{
   vprASSERT( element->getID() == "surface_viewport" );

   Projection::config(element);        // Call base class config first
}

/**
 * Recalculate the projection matrix.
 * Uses a method that needs to know the distance in the screen plane
 * from the origin (determined by the normal to the plane throught the
 * origin) to the edges of the screen.
 * This method can be used for any rectangular planar screen.
 * By adjusting the wall rotation matrix, this method can be used for
 * the general case of a rectangular screen in 3-space.
 *
 * @pre WallRotation matrix must be set correctly.
 * @pre mOrigin*'s must all be set correctly.
 * @pre eyePos is scaled by position factor.
 * @pre scaleFactor is the scale current used
 */
void SurfaceProjection::calcViewMatrix(const gmtl::Matrix44f& eyePos,
                                       const float scaleFactor)
{
   calculateOffsets();
   calcViewFrustum(eyePos, scaleFactor);

   //Coord eye_coord(eyePos);
   const gmtl::Vec3f eye_pos(gmtl::makeTrans<gmtl::Vec3f>(eyePos));             // Non-xformed pos

   // Need to post translate to get the view matrix at the position of the eye
   mViewMat = m_surface_M_base * gmtl::makeTrans<gmtl::Matrix44f>( -eye_pos );
}


/**
 * Recalculate the view frustum.
 * Uses a method that needs to know the distance in the screen plane
 * from the origin (determined by the normal to the plane throught the
 * origin) to the edges of the screen.
 * This method can be used for any rectangular planar screen.
 * By adjusting the wall rotation matrix, this method can be used for
 * the general case of a rectangular screen in 3-space.
 *
 * @pre m_base_M_surface and m_surface_M_base matrices must be set correctly.
 * @pre mOrigin*'s must all be set correctly.
 */
void SurfaceProjection::calcViewFrustum(const gmtl::Matrix44f& eyePos,
                                        const float scaleFactor)
{
   const float near_dist = mNearDist;
   const float far_dist = mFarDist;

   // Distance measurements from eye to screen/edges
   // Distance to edges is from the point on the screen plane
   // where a normal line would go through the origin.
   float eye_to_screen, eye_to_right, eye_to_left, eye_to_top, eye_to_bottom;

   // Compute transformed eye position
   // - Converts eye coords into the surface's coord system
   // Xformed position of eyes
   const gmtl::Point3f eye_surface =
      m_surface_M_base * gmtl::makeTrans<gmtl::Point3f>(eyePos);

   vprDEBUG(vrjDBG_DISP_MGR, vprDBG_HEX_LVL)
      << "SurfaceProjection::calcviewFrustum:    Base eye:" << gmtl::makeTrans<gmtl::Point3f>(eyePos)
      << "  Xformed eye:" << eye_surface
      << std::endl << vprDEBUG_FLUSH;

   // Compute dist from eye to screen/edges
   // Take into account scale since all origin to anythings are in meters
   eye_to_screen = (scaleFactor * mOriginToScreen) + eye_surface[gmtl::Zelt];
   eye_to_right =  (scaleFactor * mOriginToRight) - eye_surface[gmtl::Xelt];
   eye_to_left =   (scaleFactor * mOriginToLeft) + eye_surface[gmtl::Xelt];
   eye_to_top =    (scaleFactor * mOriginToTop) - eye_surface[gmtl::Yelt];
   eye_to_bottom = (scaleFactor * mOriginToBottom) + eye_surface[gmtl::Yelt];

   // Distances in near plane, in near plane from origin.  (Similar to above)
   // Find dists on near plane using similar triangles
   const float near_distFront = near_dist/eye_to_screen; // constant factor
   const float n_eye_to_left = eye_to_left*near_distFront;
   const float n_eye_to_right = eye_to_right*near_distFront;
   const float n_eye_to_top = eye_to_top*near_distFront;
   const float n_eye_to_bottom = eye_to_bottom*near_distFront;

   // Set frustum and calulcate the matrix
   mFrustum.set(-n_eye_to_left, n_eye_to_right,
                -n_eye_to_bottom, n_eye_to_top,
                near_dist, far_dist);

   mFocusPlaneDist = eye_to_screen;    // Needed for drawing

   vprDEBUG(vrjDBG_DISP_MGR, vprDBG_HEX_LVL)
      << "SurfaceProjection::calcWallProjection: \n\tFrustum: " << mFrustum
      << std::endl << vprDEBUG_FLUSH;

}

std::ostream& SurfaceProjection::outStream(std::ostream& out,
                                        const unsigned int indentLevel)
{
//   const int pad_width_dot(20 - indentLevel);
   out.setf(std::ios::left);

   const std::string indent_text(indentLevel, ' ');

   out << indent_text << "vrj::SurfaceProjection:\n";

   return Projection::outStream(out, indentLevel);
}

void SurfaceProjection::calculateOffsets(){
      calculateSurfaceRotation();

      m_base_M_surface=mSurfaceRotation;
      gmtl::invert(m_surface_M_base,m_base_M_surface);
      
      calculateCornersInBaseFrame();

      mOriginToScreen=-mxLLCorner[gmtl::Zelt];
      mOriginToRight=mxLRCorner[gmtl::Xelt];
      mOriginToLeft=-mxLLCorner[gmtl::Xelt];
      mOriginToTop=mxURCorner[gmtl::Yelt];
      mOriginToBottom=-mxLRCorner[gmtl::Yelt];
}

void SurfaceProjection::calculateSurfaceRotation()
{
   // Find the base vectors for the surface axiis (in terms of the base coord system)
   // With z out, x to the right, and y up
   gmtl::Vec3f x_base, y_base, z_base;
   x_base = (mLRCorner-mLLCorner);
   y_base = (mURCorner-mLRCorner);
   gmtl::cross( z_base, x_base, y_base);

   // They must be normalized
   gmtl::normalize(x_base); gmtl::normalize(y_base); gmtl::normalize(z_base);

   // Calculate the surfaceRotMat using law of cosines
   mSurfaceRotation = gmtl::makeDirCos<gmtl::Matrix44f>(x_base, y_base, z_base );
}

void SurfaceProjection::calculateCornersInBaseFrame()
{
   mxLLCorner = m_surface_M_base * mLLCorner;
   mxLRCorner = m_surface_M_base * mLRCorner;
   mxURCorner = m_surface_M_base * mURCorner;
   mxULCorner = m_surface_M_base * mULCorner;

   // Verify that they are all in the same x,y plane
   vprDEBUG(vprDBG_ALL, vprDBG_HVERB_LVL) << std::setprecision(10)
                                          << mxLLCorner[gmtl::Zelt] << " "
                                          << mxLRCorner[gmtl::Zelt] << " "
                                          << mxURCorner[gmtl::Zelt] << " "
                                          << mxULCorner[gmtl::Zelt] << "\n"
                                          << vprDEBUG_FLUSH;

#ifdef VJ_DEBUG
   // Use 1e-4f here, otherwise the floating point error can get big enough to mess this up for tracked surfaces
   vprASSERT(gmtl::Math::isEqual(mxLLCorner[gmtl::Zelt], mxLRCorner[gmtl::Zelt], 1e-4f) &&
             gmtl::Math::isEqual(mxURCorner[gmtl::Zelt], mxULCorner[gmtl::Zelt], 1e-4f) &&
             gmtl::Math::isEqual(mxLLCorner[gmtl::Zelt], mxULCorner[gmtl::Zelt], 1e-4f));
#endif
}

}
