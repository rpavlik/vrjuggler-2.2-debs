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

#ifndef _GADGET_NODE_H
#define _GADGET_NODE_H

#include <gadget/gadgetConfig.h>
#include <vpr/IO/Socket/SocketStream.h>
#include <vpr/Sync/Semaphore.h>
#include <vpr/Thread/Thread.h>
#include <gadget/Util/Debug.h>


namespace cluster
{
   class DeviceRequest;
   class Packet;
}

namespace gadget
{
   class AbstractNetworkManager;

/** \class Node Node.h gadget/Node.h
 *
 * Network node.
 */
class GADGET_CLASS_API Node
{
public:
   enum Status
   {
      DISCONNECTED   = 0,
      PENDING        = 1,
      NEWCONNECTION  = 2,
      CONNECTED      = 3
   };
   
   /**
    * Create a Node with the given attributes.
    *
    * @param name          Name of the Cluster Node from the config file
    * @param host_name     Hostname of the remote machine
    * @param port          The scoket port that we should connect to
    * @param socket_stream SocketStream used to communicate with remote machine
    * @param net_mgr       The network manager.
    */
   Node(const std::string& name, const std::string& host_name, 
               const vpr::Uint16& port, vpr::SocketStream* socket_stream,
               AbstractNetworkManager* net_mgr);

   /**
    * Shutdown the update thread and close the SocketStream.
    */
   virtual ~Node();
   
   /**
    * Display all relevant information about Node.
    */
   void debugDump(int debug_level = vprDBG_CONFIG_LVL);
   
   /**
    * Display Bandwidth statistics for the given SocketStream.
    */
   virtual void printStats(int debug_level = 1);

   /**
    * Return the name given to this node during configuration.
    */
   std::string getName()
   {
      return mName;
   }
   
   /**
    * Change the name of this node to the given value.
    */
   void setName(const std::string& name)
   {
      mName = name;
   }

   /**
    * Return the hostname of the node.
    */
   std::string getHostname()
   {
      return mHostname;
   }

   /**
    * Return the port number that we are connected to.
    */
   vpr::Uint16 getPort()
   {
      return mPort;
   }

   /**
    * Get a pointer to the SocketStream used to communicate with this node.
    */
   vpr::SocketStream* getSockStream()
   {
      return mSockStream;
   }
   
   /**
    * Set the SocketStream used to communicate with this node.
    */
   void setSockStream(vpr::SocketStream* stream)
   {
      mSockStream = stream;
   }

   /**
    * Return if we are connected to this node.
    */
   bool isConnected()
   {
      return (CONNECTED == getStatus());
   }

   /**
    * Set the current connection status to this machine.
    */
   void setStatus(int connect);

   /**
    * Get the current connection state for this node.
    */
   int getStatus()
   {
      return mStatus;
   }
   
   /**
    * Return if this node is updated or not.
    */
   bool isUpdated()
   {
      return mUpdated;
   }

   /**
    * Set the update status for this node.
    */
   void setUpdated(bool update)
   {
      mUpdated = update;
   }
   
public:
   /**
    * Start the update thread for this node.
    */
   void start();

   /**
    * Control loop for updating this thread.
    */
   void controlLoop();

   /**
    * Signal a semaphore to let the update thread fall into
    * the code to update the UserData structures.
    */
   void signalUpdate();
         
   /**
    * Signal a semaphore to signal that we are done either
    * updating the UserData or DeviceData.
    */
   void sync();
   
   /**
    * Kill the update thread.
    */
   void shutdown();

   /**
    * Get the time delta between the remote and local clock.
    */
   vpr::Uint64* getDelta()
   {
      return &mDelta;
   }
  
   /**
    * Send the given packet to this node.
    */
   bool send(cluster::Packet* out_packet);

   /**
    * Receive a packet from the network.
    */
   cluster::Packet* recvPacket();
   
protected:
   /**
    * Update this cluster node.
    */
   void update();

protected:
   std::string          mName;                  /**< Node name */
   std::string          mHostname;              /**< Host that it is connected to */
   vpr::Uint16          mPort;                  /**< Port that it is connected to */
   bool                 mRunning;               /**< Thread is running the control loop */

   vpr::SocketStream*   mSockStream;            /**< Socket used for communication to this node */      
   vpr::Mutex           mSockWriteLock;         /**< Lock writing to the SocketStream */
   vpr::Mutex           mSockReadLock;          /**< Lock reading from the SocketStream */

   vpr::Mutex           mStatusLock;            /**< Lock the isConnected value */
   int                  mStatus;                /**< States if this node is connected */
   
   vpr::Mutex           mUpdatedLock;           /**< Lock the isUpdated value */
   bool                 mUpdated;               /**< States if this node is updated */
   
   vpr::Semaphore       mUpdateTriggerSema;     /**< Semaphore trigger for UserData update  */
   vpr::Semaphore       mNodeDoneSema;          /**< Semaphore trigger for completion */
   
   vpr::Thread*         mControlThread;         /**< Update thread for this node */
   bool                 mThreadActive;          /**< Has the update thread started? */

   vpr::Uint64          mDelta;                 /**< Time delta between remote and local clocks. */
   AbstractNetworkManager*      mNetworkManager;/**< Network that should handle incoming packets. */
};

} // end namespace gadget

#endif /* _GADGET_NODE_H */
