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
package org.vrjuggler.vrjconfig.customeditors.intersense;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import org.vrjuggler.jccl.config.*;
import org.vrjuggler.jccl.editors.*;

/**
 * JTree that provides the user a view of all proxies pointing at the
 * selected Intersense Station. Also provides a method for the user
 * to create new proxies, remove existing proxies, disconnect proxies,
 * and attach new proxies new this station.
 */
public class ProxyTree extends JTree implements ActionListener
{
   /** Popup menu used when right clicking on nodes in the tree. */
   private JPopupMenu popup;

   /**
    * Construct the ProxyTree to represent the proxies pointing 
    * at a given Intersense Station.
    */
   public ProxyTree()
   {
      // Set the cell renderer and editor.
      ProxyNodeRenderer renderer = new ProxyNodeRenderer();
      ProxyNodeEditor editor = new ProxyNodeEditor(this);
      this.setCellRenderer(renderer);
      this.setCellEditor(new DefaultTreeCellEditor(this, renderer, editor));
      
      
      // Create Default TreeModel
      DefaultMutableTreeNode root_node = new DefaultMutableTreeNode("ROOT");
      DefaultMutableTreeNode button_one_node = new DefaultMutableTreeNode("Button 0");
      root_node.add(button_one_node);
      
      // For each button add a node.
      for(int i = 0 ; i < 4 ; ++i)
      {
         DefaultMutableTreeNode button_node = new DefaultMutableTreeNode("Button " + Integer.toString(i));
         root_node.add(button_node);         
      }
      
      DefaultMutableTreeNode joystick_node = new DefaultMutableTreeNode("Joystick");
      root_node.add(joystick_node); 
      
      DefaultMutableTreeNode trigger_node = new DefaultMutableTreeNode("Trigger");
      root_node.add(trigger_node); 
      
      DefaultTreeModel test_model = new DefaultTreeModel(root_node);
      DefaultMutableTreeNode currentNode = new DefaultMutableTreeNode();
      this.setModel(test_model);

      
      this.setEditable(true);
      this.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
      //XXX: DEBUG
      this.setRootVisible(true);
      //this.setRootVisible(false);
 
      // Set up the pop up menu
      JMenuItem mi;
      popup = new JPopupMenu();
      mi = new JMenuItem("Create New Proxy");
      mi.addActionListener(this);
      mi.setActionCommand("create");
      mi.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, 0));
      popup.add(mi);
      
      mi = new JMenuItem("Remove Proxy"/*, remove_icon*/);
      mi.addActionListener(this);
      mi.setActionCommand("remove");
      mi.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, 0));
      popup.add(mi);
      
      mi = new JMenuItem("Attach Existing Proxy");
      mi.addActionListener(this);
      mi.setActionCommand("attach");
      mi.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, 0));
      popup.add(mi);

      mi = new JMenuItem("Disconnect Proxy");
      mi.addActionListener(this);
      mi.setActionCommand("disconnect");
      mi.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0));
      popup.add(mi);

      popup.setOpaque(true);
      popup.setLightWeightPopupEnabled(true);


      // Add mouse listener to get the popup menu events.
      this.addMouseListener(new MouseAdapter() 
         {
            // NOTE: XWindows and Windows sense popup trigger events at
            //       different times. On windows it occurs when the mouse is
            //       released, while on XWindows is occurs when the mouse button
            //       is pressed.
         
            // Check for a popup trigger on XWindows.
            public void mousePressed(MouseEvent e)
            {
               if (e.isPopupTrigger()) 
               {
                  popup.show( (JComponent)e.getSource(), e.getX(), e.getY() );
               }         
            }
            // Check for a popup trigger on Windows.
            public void mouseReleased(MouseEvent e) 
            {
               if (e.isPopupTrigger()) 
               {
                  popup.show( (JComponent)e.getSource(), e.getX(), e.getY() );
               }
            } 
         });
   }

   /**
    * ActionListener interface to recieve events from the popup menu and the toolbar.
    *
    * @param    event   The ActionEvent created by the popup menu or toolbar.
    */
   public void actionPerformed(ActionEvent event)
   {
      // If the user selcted the insert menu item then present a list of valid proxies.
      if(event.getActionCommand().equals("attach")) 
      {
         TreePath path = this.getLeadSelectionPath();
         DefaultMutableTreeNode node = (DefaultMutableTreeNode)path.getLastPathComponent();
         Object value = node.getUserObject();
         
         // If the currently selected node in the tree is a DeviceUnit
         // then we can attach to it.
         if(value instanceof ProxyTreeModel.DeviceUnit)
         {
            ProxyTreeModel.DeviceUnit du = (ProxyTreeModel.DeviceUnit)value;
            ProxyTreeModel model = (ProxyTreeModel)getModel();
            
            // Get a list of valid proxies for this device unit.
            java.util.List elements = getBroker().getElements(model.getConfigContext());
            java.util.List proxies = ConfigUtilities.getElementsWithDefinition(elements, du.getToken());

            // Ask the user to choose a base ConfigDefinition.
            ProxyChooser chooser = new ProxyChooser();
            chooser.setProxies(proxies);
            int result = chooser.showDialog(this);

            // If the user did not cancel their choice, update the proxy to point at this DeviceUnit.
            if (result == ConfigDefinitionChooser.APPROVE_OPTION)
            {
               ConfigElement proxy = chooser.getSelectedProxy();
               proxy.setProperty("device", 0, new ConfigElementPointer(du.getDeviceName()));
               proxy.setProperty("unit", 0, new Integer(du.getDeviceNum()));
               
               // Make the tree auto select the newly attached proxy.
               selectUserObject(proxy);
            }
         }
      }
      else if(event.getActionCommand().equals("create"))
      {
         TreePath path = this.getLeadSelectionPath();
         DefaultMutableTreeNode node = (DefaultMutableTreeNode)path.getLastPathComponent();
         Object value = node.getUserObject();
         
         if(value instanceof ProxyTreeModel.DeviceUnit)
         {
            ProxyTreeModel.DeviceUnit du = (ProxyTreeModel.DeviceUnit)value;

            // Get the ConfigDefinition of the proxy we want to create.
            ConfigDefinition proxy_def = getBroker().getRepository().get(du.getToken());
            
            // Create a temporary list of ConfigDefinitions to pass to factory.
            java.util.List def_list = new ArrayList();
            def_list.add(proxy_def);
           
            // Initialize a ConfigElementFactory with the needed 
            // ConfigDefinition. And create a new ConfigElement.
            ConfigElementFactory temp_factory = new ConfigElementFactory(def_list);
            ConfigElement element = temp_factory.create("CHANGEME", proxy_def);

            // Set the properties of this new proxy.
            element.setProperty("device", 0, new ConfigElementPointer(du.getDeviceName()));
            element.setProperty("unit", 0, new Integer(du.getDeviceNum()));
      
            // Get the model for this ProxyTree.
            ProxyTreeModel model = (ProxyTreeModel)getModel();
            
            // Make sure this add goes through successfully
            if (! getBroker().add(model.getConfigContext(), element))
            {
               JOptionPane.showMessageDialog(this,
                                             "There are no configuration files active.",
                                             "Error",
                                             JOptionPane.ERROR_MESSAGE);
               return;
            }
            selectUserObject(element);
         }
      }
      else if(event.getActionCommand().equals("remove"))
      {
         // - Get selected TreeNode.
         // - Remove element in node.
         TreePath path = this.getLeadSelectionPath();
         DefaultMutableTreeNode node = (DefaultMutableTreeNode)path.getLastPathComponent();
         Object value = node.getUserObject();
         
         if(value instanceof ConfigElement)
         {
            ConfigElement element = (ConfigElement)value;
            ProxyTreeModel model = (ProxyTreeModel)getModel();
            
            // Make sure this add goes through successfully
            if (! getBroker().remove(model.getConfigContext(), element))
            {
               JOptionPane.showMessageDialog(this,
                                             "There are no configuration files active.",
                                             "Error",
                                             JOptionPane.ERROR_MESSAGE);
               return;
            }
         }
      }
      else if(event.getActionCommand().equals("disconnect"))
      {
         // - If node contains a ConfigElement
         //  - Set device to "" and unit to 0
         TreePath path = this.getLeadSelectionPath();
         DefaultMutableTreeNode node = (DefaultMutableTreeNode)path.getLastPathComponent();
         Object value = node.getUserObject();
         
         if(value instanceof ConfigElement)
         {
            ConfigElement element = (ConfigElement)value;
            element.setProperty("device", 0, new ConfigElementPointer(""));
            element.setProperty("unit", 0, new Integer(0));
         }
      }
   }

   /**
    * Expand or collapse the entire tree.
    *
    * @param expand     true to expand, false to collapse 
    */
   public void expandAll(boolean expand)
   {
      TreeNode root = (TreeNode)getModel().getRoot();

      // Traverse tree from root
      expandAll(new TreePath(root), expand);
   }

   /**
    * Expand or collapse the entire tree under the given path.
    *
    * @param parent     path to the parent to expand/collapse under
    * @param expand     true to expand, false to collapse 
    */
   public void expandAll(TreePath parent, boolean expand)
   {
      // Traverse children
      TreeNode node = (TreeNode)parent.getLastPathComponent();
      if (node.getChildCount() >= 0)
      {
         for (Enumeration e=node.children(); e.hasMoreElements(); )
         {
            TreeNode n = (TreeNode)e.nextElement();
            TreePath path = parent.pathByAddingChild(n);
            expandAll(path, expand);
         }
      }

      // Expansion or collapse must be done bottom-up
      if(expand)
      {
         expandPath(parent);
      }
      else
      {
         collapsePath(parent);
      }
   }
   
   /**
    * Selects the path to the given ConfigElement in the ProxyTree.
    */
   public void selectUserObject(Object obj)
   {
      ProxyTreeModel model = (ProxyTreeModel)getModel();
      java.util.List element_nodes = model.getNodesFor(obj);
      if (element_nodes.size() > 0)
      {
         TreeNode element_node = (TreeNode)element_nodes.get(0);
         TreePath path = new TreePath(model.getPathToRoot(element_node));
         setSelectionPath(path);
      } 
   }

   
   /** Reference to the ConfigBroker used in this object. */
   private ConfigBroker mBroker = null;
   
   /**
    * Gets a handle to the configuration broker.
    */
   private ConfigBroker getBroker()
   {
      if (mBroker == null)
      {
         synchronized (this)
         {
            if (mBroker == null)
            {
               mBroker = new ConfigBrokerProxy();
            }
         }
      }
      return mBroker;
   }

   /**
    * Custom TreeCellRenderer for proxies.
    */
   public class ProxyNodeRenderer extends DefaultTreeCellRenderer
   {
      /** List of Icons used for digital nodes. */
      private java.util.List mDigitalIcons = new ArrayList();
      
      /** List of Icons used for analog nodes. */
      private java.util.List mAnalogIcons = new ArrayList();

      /** Icon used for position nodes. */
      private Icon mPositionIcon = null;

      /** Icon used for proxies nodes. */
      private Icon mProxyIcon = null;
     
      /**
       * Constructor.
       */
      public ProxyNodeRenderer()
      {
         ClassLoader loader = getClass().getClassLoader();
         Icon temp_icon;
        
         // Load the digital icons.
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/b0_yellow.png"));
         mDigitalIcons.add(temp_icon);
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/b1_red.png"));
         mDigitalIcons.add(temp_icon);
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/b2_green.png"));
         mDigitalIcons.add(temp_icon);
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/b3_blue.png"));
         mDigitalIcons.add(temp_icon);
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/b4_joystick.png"));
         mDigitalIcons.add(temp_icon);
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/b5_trigger.png"));
         mDigitalIcons.add(temp_icon);
         
         // Load the analog icons.
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/horiz.gif"));
         mAnalogIcons.add(temp_icon);
         temp_icon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/vert.gif"));
         mAnalogIcons.add(temp_icon);
        
         mPositionIcon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/position16.png"));
         mProxyIcon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/proxy.png"));
      }
      
      /**
       * Creates new TreeNodeRenderer for given value.
       *
       * @param tree    JTree which is asking for renderer.
       * @param value   Value of node being renderer.
       * @param expanded Wether the node is expanded.
       * @param leaf    Wether the node is a leaf node.
       * @param row     The row that this node is in.
       * @param hasFocus Wether this node is selected.
       */
      public Component getTreeCellRendererComponent(JTree tree, Object value, 
                                                    boolean selected, boolean expanded,
                                                    boolean leaf, int row, boolean hasFocus)
      {
         DefaultMutableTreeNode node = (DefaultMutableTreeNode)value;
       
         // If the node has a ConfigElement we know that it is a proxy.
         if(node.getUserObject() instanceof ConfigElement)
         {
            ConfigElement elm = (ConfigElement)node.getUserObject();
            
            // NOTE: DefaultTreeCellRenderer extends JLabel and returns 
            // itself, if we call the following method to set everything up
            // we can then set the Icon to whatever we want and return this.
            super.getTreeCellRendererComponent( tree, elm.getName(), selected, expanded, leaf,row, hasFocus);

            setIcon(mProxyIcon);
         }
         else
         {
            // Get the default renderer for the passed value because we know that the UserObject is
            // either a ProxyType or a DeviceUnit which both implement the toString method.
            super.getTreeCellRendererComponent(tree, value, selected, expanded, leaf, row, hasFocus);

            // Use the appropriate Icon.
            if(node.getUserObject() instanceof ProxyTreeModel.DeviceUnit)
            {
               ProxyTreeModel.DeviceUnit du = (ProxyTreeModel.DeviceUnit)node.getUserObject();
            
               // Set the correct Icon for this node in the tree.
               if(du.getToken().equals("digital_proxy"))
               {
                  setIcon((Icon)mDigitalIcons.get(du.getDeviceNum()%6));
               }
               else if(du.getToken().equals("analog_proxy"))
               {
                  setIcon((Icon)mAnalogIcons.get(du.getDeviceNum()%2));
               }
               else if(du.getToken().equals("position_proxy"))
               {
                  setIcon(mPositionIcon);
               }
            }
         }
         
         return this;
      }
   }

   /**
    * Custom TreeCellEditor for proxies.
    */
   public class ProxyNodeEditor extends DefaultTreeCellEditor
   {
      /**
       * Constructor that takes a reference to the JTree that we will be editing.
       * This is required so that we can determine if the selected node is
       * editable.
       */
      public ProxyNodeEditor(JTree tree)
      {
         super(tree, (DefaultTreeCellRenderer)tree.getCellRenderer());

         ClassLoader loader = getClass().getClassLoader();
         editingIcon = new ImageIcon(loader.getResource("org/vrjuggler/vrjconfig/customeditors/intersense/images/position16.png"));
         mTree = tree;
      }
   
      /**
       * Returns a component that will be used to edit the name of the
       * ConfigElement.
       */
      public Component getTreeCellEditorComponent(JTree tree,
                                               Object value,
                                               boolean isSelected,
                                               boolean expanded,
                                               boolean leaf,
                                               int row)
      {
         // Get a reference to the selected TreeNode.
         DefaultMutableTreeNode node = (DefaultMutableTreeNode)value;
       
         if(!(node.getUserObject() instanceof ConfigElement))
         {
            throw new IllegalArgumentException("Error: "
                  + "The selected node contains an object with type:" 
                  + node.getUserObject().getClass());
         }
        
         // Get a reference to the selected ConfigElement.
         mElement = (ConfigElement)node.getUserObject();
         mTextField = new JTextField(mElement.getName());
         mTree = tree;
        
         
         // Specify what should happen when done editing.
         mTextField.addActionListener(new ActionListener()
         {
            public void actionPerformed(ActionEvent evt)
            {
               // Force the focus to be lost.
               //mTextField.transferFocusUpCycle();
               // Force the focus to be transfered to the next component.
               //mTextField.transferFocus();
               
               // This is no longer needed since the above line will force a
               // focusLostEvent. But I have choosen to leave this line here in
               // case it might become useful later.
               stopCellEditing();
               mTree.clearSelection();
            }
         });
        
         // Add a focus listener to save changes to the name when we lose focus.
         mTextField.addFocusListener(new FocusAdapter()
         {
            public void focusLost(FocusEvent evt)
            {
               stopCellEditing();
            }
         });
         
         return mTextField;
      }

      /**
       * Get the value of the cell being edited.
       */
      public Object getCellEditorValue()
      {
         return mElement;
      }
     
      /**
       * Return wether we should be allowed to edit the selected cell.
       */
      public boolean isCellEditable(EventObject evt)
      {
         TreePath current_selection = mTree.getSelectionPath();
         if (current_selection != null) 
         {
            // Get the currently selected node.
            DefaultMutableTreeNode current_node = (DefaultMutableTreeNode)
               (current_selection.getLastPathComponent());
            
            // If the selected node is not null and is a ConfigElement
            // then we should allow editing.
            if (current_node != null &&
               (current_node.getUserObject() instanceof ConfigElement))
            {
               return true;
            }
         } 
         return false;
      }

      /**
       * We should always select the cell.
       */
      public boolean shouldSelectCell(EventObject evt)
      { return true; }

      /**
       * When editing stops we should set the new name.
       */
      public boolean stopCellEditing()
      {
         mElement.setName(mTextField.getText());      
         return true; 
      }
      public void cancelCellEditing()
      {;}
      public void addCellEditorListener(CellEditorListener l)
      {;}
      public void removeCellEditorListener(CellEditorListener l)
      {;}

      /** JTree that we are editing. */
      private JTree           mTree       = null;

      /** JTextField that will be used as the editor. */
      private JTextField      mTextField  = null;

      /** ConfigElement that we are editing the name of. */
      private ConfigElement   mElement    = null;
   }
}
