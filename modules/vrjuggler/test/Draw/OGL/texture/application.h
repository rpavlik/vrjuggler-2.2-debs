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

#ifndef _TEXTURE_APP_
#define _TEXTURE_APP_

#include <vrj/vrjConfig.h>          // needed by other juggler classes

// C++ and STL (standard template library - http://www.sgi.com/Technology/STL)
#include <iostream> // for std::cout
#include <vector>   // for std::vector

// C
#include <math.h>

// OpenGL
#if defined(__APPLE__)
#  include <OpenGL/gl.h>  // OpenGL API
#  include <OpenGL/glu.h> // GLU functions (extended opengl stuff)
#else
#  include <GL/gl.h>  // OpenGL API
#  include <GL/glu.h> // GLU functions (extended opengl stuff)
#endif

// VR juggler
#include <vpr/Util/Interval.h>
#include <gadget/Type/PositionInterface.h>
#include <vrj/Draw/OGL/GlApp.h>     // base OpenGL application API
#include <vrj/Draw/OGL/GlContextData.h> // for OpenGL resource IDs

// texture application objects
#include "renderGeometry.h"  // render interleaved vertex array data
#include "cubeGeometry.h"    // cube geometry data
#include "Image.h"           // Image class
#include "Texture.h"         // Texture class
#include "hexImage.h"        // the floor texture's image data
#include "squareImage.h"     // the cube texture's image data
#include "renderTexture.h"   // render Texture class to OpenGL hardware

//: VR Juggler application demonstration to show you
//  how to do texturing in an OpenGL juggler application
class TextureDemoApplication : public vrj::GlApp
{
public:
   //: Constructor
   TextureDemoApplication( vrj::Kernel* kern );

   //: destructor
   virtual ~TextureDemoApplication();

   virtual void init();

   //: Called immediately upon opening a new OpenGL context
   // (called for every window that is opened)
   // put your opengl resource allocation here...
   virtual void contextInit();

   //: Called immediately upon closing an OpenGL context
   // (called for every window that is closed)
   // put your opengl deallocation here...
   virtual void contextClose();

   virtual void bufferPreDraw();

   //: Function to "draw" the scene
   //  put your opengl draw functions here...
   //  PRE: OpenGL state has correct transformation and buffer selected
   //  POST: The current scene has been drawn
   virtual void draw();

   // Function called before updating trackers but after the frame is drawn
   // do calculations here...
   virtual void postFrame();

// private stuff!
private:
   float x;

   // use the type: GlContextData<ResourceID>
   // to store your OpenGL texture object and display list IDs
   // i.e. GlContextData<ResourceID>  mTexObj;
   class ResourceID
   {
   public:
      ResourceID() : id( -1 )
      {
      }
      int id;
   };

   // helper functions to make your code easier to read
   // i.e. hides all the static_cast and template lengthyness
   static void setTexObjID( Texture& t,
                            vrj::GlContextData<ResourceID>& texObjectID )
   {
      void* data = reinterpret_cast<void*>( &texObjectID );
      t.setUserData( data );
   }
   static int& getTexObjID( Texture& t )
   {
      vrj::GlContextData<ResourceID>* texObject = static_cast<vrj::GlContextData<ResourceID>* >( t.userData() );
      return (*texObject)->id;
   }

   // Cube objects:
   Texture                        mCubeTexture;
   vrj::GlContextData<ResourceID> mCubeTextureObj;
   cubeGeometry                   mCubeGeometry;
   vrj::GlContextData<ResourceID> mCubeDisplayList;

   // Floor:
   Texture                        mFloorTexture;
   vrj::GlContextData<ResourceID> mFloorTextureObj;

   gadget::PositionInterface mWand;

   // timer
   vpr::Interval mLastFrameTime;
};


#endif
