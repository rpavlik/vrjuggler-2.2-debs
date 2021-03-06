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

#include <vrj/Draw/OGL/Config.h>

#include <vrj/Draw/OGL/GlDrawManager.h>
#include <vrj/Draw/OGL/GlApp.h>


namespace vrj
{

// NOTE: These member functions are implemented here rather than being
// inlined within GlApp.h so that vrj::GlApp is properly compiled into the
// VR Juggler OpenGL Draw Manager library.  Sometimes, full inlining is a
// bad thing.

GlApp::GlApp(Kernel* kern) : App(kern)
{
   /* Do nothing. */ ;
}


GlApp::~GlApp()
{
   /* Do nothing. */ ;
}

DrawManager* GlApp::getDrawManager()
{
   return GlDrawManager::instance();
}

}
