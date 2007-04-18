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

#ifndef _GADGET_MOUSE_EVENT_H_
#define _GADGET_MOUSE_EVENT_H_

#include <gadget/gadgetConfig.h>

#include <gadget/Type/KeyboardMouse/Event.h>
#include <gadget/Type/KeyboardMouse/MouseEventPtr.h>
#include <gadget/Type/KeyboardMouse/Keys.h>
#include <gadget/Type/KeyboardMouse/EventFactory.h>


namespace gadget
{
GADGET_REGISTER_EVENT_CREATOR( MouseEvent, MouseButtonPressEvent );
GADGET_REGISTER_EVENT_CREATOR( MouseEvent, MouseButtonReleaseEvent );
GADGET_REGISTER_EVENT_CREATOR( MouseEvent, MouseMoveEvent );

/** \class MouseEvent MouseEvent.h gadget/Type/KeyboardMouse/MouseEvent.h
 *
 * Mouse event class.  This captures the state of the mouse when any mouse
 * event occurs.  This includes button presses, button releases, and mouse
 * motion.
 */
class GADGET_CLASS_API MouseEvent : public gadget::Event
{
public:
   /**
    * Initializes data members.
    *
    * @param type          The type of this event (a mouse button press, a
    *                      mouse button release, or a mouse move).
    * @param button        The button that generated the event.
    * @param x             The X coordinate of the mouse within the window.
    * @param y             The Y coordinate of the mouse within the window.
    * @param globalX       The X coordiante of the mouse relative to the root
    *                      window (i.e., the desktop).
    * @param globalY       The Y coordiante of the mouse relative to the root
    *                      window (i.e., the desktop).
    * @param state         The mask of mouse buttons and any modifiers being
    *                      pressed.  This should be constructed using the
    *                      bitwise OR of gadget::ModifierMask and
    *                      gadget::ButtonMask values.
    * @param time          The time at which this event occurred.  This should
    *                      be as accurate as possible,  preferabbly acquired
    *                      from the operating system or windowing system event
    *                      data structure.  The time at which the event was
    *                      processed is not an acceptable value.
    *
    * @see gadget::ModifierMask, gadget::ButtonMask
    */
   MouseEvent(const gadget::EventType type, const gadget::Keys button,
              const int x, const int y, const int globalX, const int globalY,
              const int state, const unsigned long time);

   /**
    * Default constructor needed in order to use the templated EventFactory 
    * without modification. The EventFactory was needed to allow the 
    * correct subtype of Event, MouseEvent in this case, to be created 
    * during de-serialization.
    */
   MouseEvent();

   ~MouseEvent()
   {
      /* Do nothing. */ ;
   }

   /**
    * Get the mouse button that was pressed while generating this event.
    */
   const gadget::Keys getButton() const
   {
      return mButton;
   }

   /**
    * Returns the X coordinate of the mouse pointer relative to the window.
    */
   const int getX() const
   {
      return mRelativeX;
   }

   /**
    * Returns the Y coordinate of the mouse pointer relative to the window.
    */
   const int getY() const
   {
      return mRelativeY;
   }

   /**
    * Returns the X coordinate of the mouse pointer relative to the root
    * window (i.e., the desktop).
    */
   const int getGlobalX() const
   {
      return mGlobalX;
   }

   /**
    * Returns the Y coordinate of the mouse pointer relative to the root
    * window (i.e., the desktop).
    */
   const int getGlobalY() const
   {
      return mGlobalY;
   }

   /**
    * Returns the state of the mouse buttons and keyboard modifier keys
    * (CTRL, ALT, and SHIFT).
    *
    * @see gadget::ModifierMask, gadget::ButtonMask
    */
   const int getState() const
   {
      return mState;
   }

   /**
    * Serializes this object into the given object writer.
    *
    * @param writer The object writer to which this object will be serialized.
    *
    * @throw vpr::IOException is thrown if serialization fails.
    */
   virtual void writeObject(vpr::ObjectWriter* writer);

   /**
    * De-serializes this object.
    *
    * @param reader The object reader from which this object will be
    *               de-serialized.
    *
    * @throw vpr::IOException is thrown if de-serialization fails.
    */
   virtual void readObject(vpr::ObjectReader* reader);

private:
   gadget::Keys mButton;        /**< The button associated with this event. */
   int          mRelativeX;     /**< Relative X position. */
   int          mRelativeY;     /**< Relative Y position. */
   int          mGlobalX;       /**< Global X position. */
   int          mGlobalY;       /**< Global Y position. */
   int          mState;         /**< Mouse button and modifier key state. */
};

} // End of gadget namespace


#endif /* _GADGET_MOUSE_EVENT_H_ */
