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

#include <gmtl/Matrix.h>
#include <gmtl/Xforms.h>
#include <digitalGloveApp.h>


// Execute any initialization needed before the API is started.
// Initialize VR Juggler device interfaces here.
void digitalGloveApp::init()
{
   //std::cout<<"digitalGloveApp::init()"<<std::flush;

   mGlove.init("VJGlove");

   // for the digital glove fingers.
   mLeftThumb.init("PinchLeftThumb");
   mLeftIndex.init("PinchLeftIndex");
   mLeftMiddle.init("PinchLeftMiddle");
   mLeftRing.init("PinchLeftRing");
   mLeftPinky.init("PinchLeftPinky");
   mRightThumb.init("PinchRightThumb");
   mRightIndex.init("PinchRightIndex");
   mRightMiddle.init("PinchRightMiddle");
   mRightRing.init("PinchRightRing");
   mRightPinky.init("PinchRightPinky");

   //std::cout<<" --- done\n"<<std::flush;
}

// Function called after tracker update but before start of drawing.  Do
// calculations and state modifications here.
void digitalGloveApp::preFrame()
{
  //std::cout<<"digitalGloveApp::preFrame()\n"<<std::flush;

    std::cout<<mLeftThumb->getData()
             <<mLeftIndex->getData()
             <<mLeftMiddle->getData()
             <<mLeftRing->getData()
             <<mLeftPinky->getData()<<"."
             <<mRightThumb->getData()
             <<mRightIndex->getData()
             <<mRightMiddle->getData()
             <<mRightRing->getData()
             <<mRightPinky->getData()<<"\n"<<std::flush;
}

void digitalGloveApp::bufferPreDraw()
{
   // Clear the scene
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT);
}

void digitalGloveApp::myDraw()
{
    //cout<<"digitalGloveApp::myDraw()\n"<<flush;

   glClear(GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);

   int finger;

      // -- Draw box on wand --- //
   gmtl::Matrix44f finger_matrix;

   // Draw green balls on finger tips
   glColor3f(0.0f, 1.0f, 0.0f);
   glPushMatrix();
   {
      for ( finger = gadget::GloveData::INDEX;
            finger <= gadget::GloveData::PINKY;
            ++finger )
      {
         glPushMatrix();
            finger_matrix =
               mGlove->getTipTransform((gadget::GloveData::GloveComponent) finger);
            glMultMatrixf(finger_matrix.mData);
            drawSphere((0.1f*(1.0f/12.0f)), 4, 4);
         glPopMatrix();
      }
   }
   glPopMatrix();

   // Draw red rays out of fingers
   // I am not doing this in the most efficient way.
   // I am just trying to test the vector functions
   glColor3f(1.0f, 0.0f, 0.0f);
   glPushMatrix();
   {
      for ( finger = gadget::GloveData::INDEX;
            finger <= gadget::GloveData::PINKY;
            ++finger )
      {
         glPushMatrix();
            gmtl::Point3f origin(0,0,0);    // Base of the vector
            finger_matrix =
               mGlove->getTipTransform((gadget::GloveData::GloveComponent) finger);
            origin = finger_matrix * origin;     // Go to new coord system
            gmtl::Point3f end = origin + (0.25f * mGlove->getTipVector((gadget::GloveData::GloveComponent) finger));
            drawLine(origin, end);
         glPopMatrix();
      }
   }
   glPopMatrix();
}

void digitalGloveApp::initGLState()
{
   //cout<<"digitalGloveApp::initGLState()\n"<<flush;

   GLfloat light0_ambient[] = { .2f,  .2f,  .2f,  1.0f };
   GLfloat light0_diffuse[] = { 1.0,  1.0,  1.0,  1.0};
   GLfloat light0_specular[] = { 1.0,  1.0,  1.0,  1.0};
   GLfloat light0_position[] = {2000.0, 1000.0, 100.0, 1.0};
   GLfloat light0_direction[] = {-100, -100.0, -100.0};

   GLfloat mat_ambient[] = { 0.7f, 0.7f,  0.7f,  1.0f };
   GLfloat mat_diffuse[] = { 1.0f,  0.5f,  0.8f,  1.0f };
   GLfloat mat_specular[] = { 1.0,  1.0,  1.0,  1.0};
   GLfloat mat_shininess[] = { 50.0};
//   GLfloat mat_emission[] = { 1.0,  1.0,  1.0,  1.0};
   GLfloat no_mat[] = { 0.0,  0.0,  0.0,  1.0};

   glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR,  light0_specular);
   glLightfv(GL_LIGHT0, GL_POSITION,  light0_position);
   glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);

   glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
   glMaterialfv( GL_FRONT,  GL_DIFFUSE, mat_diffuse );
   glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
   glMaterialfv( GL_FRONT,  GL_SHININESS, mat_shininess );
   glMaterialfv( GL_FRONT,  GL_EMISSION, no_mat);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_COLOR_MATERIAL);
   glShadeModel(GL_SMOOTH);
}
