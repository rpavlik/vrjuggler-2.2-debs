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

package org.vrjuggler.jccl.config.undo;

import javax.swing.undo.AbstractUndoableEdit;
import javax.swing.undo.CannotUndoException;
import javax.swing.undo.CannotRedoException;

import org.vrjuggler.jccl.config.ConfigElement;


/**
 * Undoable edit type for the removal of a value from a variable-valued
 * config element property.
 */
public class ConfigElementPropertyValueRemoveEdit
   extends AbstractUndoableEdit
{
   public ConfigElementPropertyValueRemoveEdit(ConfigElement elm, String name,
                                               int removedIndex, Object value)
   {
      super();
      mElement      = elm;
      mPropName     = name;
      mRemovedIndex = removedIndex;
      mPropValue    = value;
   }

   public void undo() throws CannotUndoException 
   {      
      super.undo();
      mElement.addProperty(mPropName, mRemovedIndex, mPropValue);
   }

   public void redo() throws CannotRedoException
   {
      super.redo();
      mElement.removeProperty(mPropName, mRemovedIndex);
   }

   private ConfigElement mElement;
   private String        mPropName;
   private int           mRemovedIndex;
   private Object        mPropValue;
}
