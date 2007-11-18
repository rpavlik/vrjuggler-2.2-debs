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

#ifndef _VRJ_GL_WINDOW_COCOA_H_
#define _VRJ_GL_WINDOW_COCOA_H_

#include <vrj/Draw/OGL/Config.h>

#include <gadget/Devices/KeyboardMouseDevice/InputAreaCocoa.h>

#include <vpr/vpr.h>
#include <vrj/Draw/OGL/GlWindow.h>


// Allow this header to be included by C++ and Objective-C++ code.
#if defined(__OBJC__)
@class NSString;
@class NSLock;
@class NSOpenGLView;
#else
class NSString;
class NSLock;
class NSOpenGLView;
#endif

namespace vrj
{

/**
 *
 * @since 2.1.21
 */
class GlWindowCocoa
   : public vrj::GlWindow
   , public gadget::InputAreaCocoa
{
public:
   GlWindowCocoa();

   virtual ~GlWindowCocoa();

   virtual bool open();

   virtual bool close();

   virtual bool makeCurrent();

   virtual void configWindow(vrj::DisplayPtr displayWindow);

   virtual void swapBuffers();

   void acquireRenderLock();

   void releaseRenderLock();

   void setInStereo(const bool inStereo)
   {
      mInStereo = inStereo;
   }

   /**
    * Updates the bounds of this window to be the given values.
    *
    * @see gadget::InputAreaCocoa::updateOriginAndSize()
    * @see vrj::GlWindow::updateOriginSize()
    *
    * @since 2.2.1
    */
   void updateBounds(const float x, const float y, const float width,
                     const float height);

   /**
    * Informs this object that the NSWindow object that we created previously
    * has been closed by an external entity and hence deallocated. By invoking
    * this method with the \p isOpen parameter set to false, the caller is
    * telling this object that it no longer has an NSWindow object and that it
    * is basically defunct.
    *
    * @post \c mWindowIsOpen is set to the value of \c isOpen.
    */
   void setWindowOpen(const bool isOpen);

private:
   /**
    * Waits until it is safe to open a window and then does so. As soon as
    * the window is open, the method exits.
    *
    * @note This blocks on the static member \c sWinOpenLock from
    *       gadget::InputAreaCocoa.
    */
   void waitAndOpen();

   /**
    * Opens the window.
    *
    * @post \c mWindowOpen is set to \c true.
    */
   void finishOpen();

   vpr::Thread* mThread;

   unsigned int  mScreen;
   NSString*     mWindowName;
   NSLock*       mRenderLock;
   NSOpenGLView* mGlView;
};

}


#endif /* _VRJ_GL_WINDOW_COCOA_H_ */
