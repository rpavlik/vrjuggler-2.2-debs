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

package org.vrjuggler.tweek.gui;

import java.awt.*;
import java.beans.Beans;
import java.util.Vector;
import javax.swing.*;
import javax.swing.tree.TreeModel;
import org.vrjuggler.tweek.TweekCore;
import org.vrjuggler.tweek.beandelivery.*;
import org.vrjuggler.tweek.beans.*;
import org.vrjuggler.tweek.event.*;
import org.vrjuggler.tweek.net.CommunicationEvent;
import org.vrjuggler.tweek.net.CommunicationListener;
import org.vrjuggler.tweek.net.corba.*;
import org.vrjuggler.tweek.text.MessageDocument;
import tweek.*;


/**
 * The main GUI pane used in Tweek.  It is used as a container for the
 * JavaBeans loaded at runtime.  Thus, its main purpose is to lay out those
 * Swing components nicely.  This code was initially generated by JBuilder,
 * though little of the original generated code remains.  The Beans themselves
 * are organized in a BeanTree object.
 *
 * @version $Revision$
 */
public class BeanContainer
   extends JPanel
   implements BeanInstantiationListener, BeanDeliverySubjectUpdateListener
{
   // ========================================================================
   // Public methods.
   // ========================================================================

   public BeanContainer()
   {
      // Before we register ourselves as a listener for Bean instatiation
      // events, query the list of existing Beans.  We need to make sure that
      // we know about any existing Beans that implement the various listener
      // interfaces this class manages.
      java.util.List known_beans =
         BeanRegistry.instance().getBeansOfType(TweekBean.class.getName());

      java.util.Iterator i = known_beans.iterator();
      Object bean;

      while ( i.hasNext() )
      {
         bean = ((TweekBean) i.next()).getBean();

         // Check
         if ( bean instanceof CommunicationListener )
         {
            addCommunicationListener((CommunicationListener) bean);
         }

         if ( bean instanceof UserLevelChangeListener )
         {
            addUserLevelChangeListener((UserLevelChangeListener) bean);
         }
      }

      BeanInstantiationCommunicator.instance().addBeanInstantiationListener(this);

      try
      {
         jbInit();
      }
      catch(Exception e)
      {
         e.printStackTrace();
      }
   }

   public void replaceViewer(org.vrjuggler.tweek.beans.BeanModelViewer v)
   {
      if ( v != null )
      {
         this.removeAll();
         this.add(v.getViewer(), BorderLayout.CENTER);
         this.repaint();
      }
   }

   public synchronized void addUserLevelChangeListener(UserLevelChangeListener listener)
   {
      MessageDocument doc = TweekCore.instance().getMessageDocument();
      doc.printStatusnl("Adding new UserLevelChangeListener");
      mLevelListeners.add(listener);
   }

   public synchronized void removeUserLevelChangeListener(UserLevelChangeListener listener)
   {
      mLevelListeners.removeElement(listener);
   }

   public void fireUserLevelChange(int old_level, int new_level)
   {
      UserLevelChangeEvent e = new UserLevelChangeEvent(this, old_level,
                                                        new_level);

      UserLevelChangeListener l = null;
      Vector listeners;

      synchronized (this)
      {
         listeners = (Vector) mLevelListeners.clone();
      }

      for ( int i = 0; i < listeners.size(); i++ )
      {
         l = (UserLevelChangeListener) listeners.elementAt(i);
         l.userLevelChange(e);
      }
   }

   public synchronized void addCommunicationListener(CommunicationListener listener)
   {
      MessageDocument doc = TweekCore.instance().getMessageDocument();
      doc.printStatusnl("Adding new CommunicationListener");
      mCommListeners.add(listener);
   }

   public synchronized void removeCommunicationListener(CommunicationListener listener)
   {
      mCommListeners.removeElement(listener);
   }

   public void fireConnectionEvent(CorbaService corba_if)
   {
      addBeanDeliveryObserver(corba_if);

      CommunicationEvent e =
         new CommunicationEvent(this, CommunicationEvent.CONNECT, corba_if);

      CommunicationListener l = null;
      Vector listeners;

      synchronized (this)
      {
         listeners = (Vector) mCommListeners.clone();
      }

      for ( int i = 0; i < listeners.size(); i++ )
      {
         l = (CommunicationListener) listeners.elementAt(i);

         try
         {
            l.connectionOpened(e);
         }
         catch (Exception open_ex)
         {
            System.err.println("Caught exception when notifying listener of opened connection");
//            System.err.println("Error: " + open_ex.getMessage());
            open_ex.printStackTrace();
         }
      }
   }

   public void fireDisconnectionEvent(CorbaService corba_if)
   {
      removeBeanDeliveryObserver(corba_if);

      CommunicationEvent e = new CommunicationEvent(this,
                                                    CommunicationEvent.DISCONNECT,
                                                    corba_if);

      CommunicationListener l = null;
      Vector listeners;

      synchronized (this)
      {
         listeners = (Vector) mCommListeners.clone();
      }

      for ( int i = 0; i < listeners.size(); i++ )
      {
         l = (CommunicationListener) listeners.elementAt(i);

         try
         {
            l.connectionClosed(e);
         }
         catch (Exception close_ex)
         {
            System.err.println("Caught exception when notifying listener of closed connection");
//            System.err.println("Error: " + close_ex.getMessage());
            close_ex.printStackTrace();
         }
      }
   }

   /**
    * Implementation of BeanInstantiationListener.beanInstantiation().  Upon
    * instantiation of a JavaBean by the BeanTree, the Bean is inspected to
    * determine if it implements the CommunicationListener interface.
    * If it is so determined, the Bean is automatically added to the
    * collection of listeners for CommunicationEvents.
    */
   public void beanInstantiated(BeanInstantiationEvent event)
   {
      Object bean = event.getTweekBean().getBean();

      try
      {
         if ( Beans.isInstanceOf(bean, Class.forName("org.vrjuggler.tweek.net.CommunicationListener")) )
         {
            addCommunicationListener((CommunicationListener) bean);
         }

         if ( Beans.isInstanceOf(bean, Class.forName("org.vrjuggler.tweek.event.UserLevelChangeListener")) )
         {
            addUserLevelChangeListener((UserLevelChangeListener) bean);
         }
      }
      // This better not happen (i.e., org.vrjuggler.tweek.net.* and
      // org.vrjuggler.tweek.beans.* had better be there) ...
      catch (ClassNotFoundException e)
      {
         e.printStackTrace();
      }
   }

   public void beansAdded(BeanDeliverySubjectUpdateEvent e)
   {
      org.vrjuggler.tweek.TweekCore.instance().loadBeans(e.getNewBeans());
   }

   public void activeBeanChanged(BeanDeliverySubjectUpdateEvent e)
   {
      if ( e.hasActiveBean() )
      {

      }
   }

   private void jbInit() throws Exception
   {
      this.setLayout(mContainerLayout);
   }

   private void addBeanDeliveryObserver(CorbaService corbaService)
   {
      SubjectManager subj_mgr = corbaService.getSubjectManager();

      // The hard-coded string used here is the same as that used by the
      // automatically registered BeanDeliverySubject servant.  Any other
      // such servants are not of interest to us.
      Subject subject = subj_mgr.getSubject("TweekBeanPusher");

      // Try to narrow the Subject object to a SliderSubject object.  If this
      // fails, it throws a CORBA BAD_PARAM exception.  In that case, we open
      // a dialog box saying that the narrowing failed.
      try
      {
         // Narrow the subject.
         BeanDeliverySubject push_subject =
            BeanDeliverySubjectHelper.narrow(subject);

         // Create the observer.
         BeanDeliveryObserverImpl push_observer =
            new BeanDeliveryObserverImpl(push_subject);

         // Store our new observer in the collection of other observers.  We
         // will need it again later, and we must support multiple observers
         // of BeanDeliverySubject objects.
         mBeanDeliveryObserverMap.put(corbaService, push_observer);

         // Declare our interest in events relating to push_subject.
         push_observer.addBeanDeliverySubjectUpdateListener(this);

         // Attach our observer to the subject.
         corbaService.registerObject(push_observer,
                                     "TweekBeanPusher-" + corbaService.getNameServiceURI());
         push_subject.attach(push_observer._this());

         // Synchronize with the subject's current state information.
         push_observer.update();
      }
      catch (org.omg.CORBA.BAD_PARAM corba_ex)
      {
         JOptionPane.showMessageDialog(null,
                                       "Failed to narrow subject to BeanDeliverySubject",
                                       "BeanDeliverySubject Narrow Error",
                                       JOptionPane.ERROR_MESSAGE);
      }
   }

   private void removeBeanDeliveryObserver(CorbaService corbaService)
   {
      BeanDeliveryObserverImpl push_observer =
         (BeanDeliveryObserverImpl) mBeanDeliveryObserverMap.get(corbaService);

      if ( null != push_observer )
      {
         // Clean up before disconnection.
         push_observer.removeBeanDeliverySubjectUpdateListener(this);
         push_observer.detach();

         // Remove the key for corbaService.  We cannot use it again.
         mBeanDeliveryObserverMap.remove(corbaService);
      }
   }

   // ========================================================================
   // Private data members.
   // ========================================================================

   private Vector mCommListeners  = new Vector();
   private Vector mLevelListeners = new Vector();

   private java.util.Map mBeanDeliveryObserverMap = new java.util.HashMap();

   private TreeModel mDataModel = null;
   private BorderLayout mContainerLayout = new BorderLayout();
}
