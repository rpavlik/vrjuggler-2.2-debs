/****************** <VPR heading BEGIN do not edit this line> *****************
 *
 * VR Juggler Portable Runtime
 *
 * Original Authors:
 *   Allen Bierbaum, Patrick Hartling, Kevin Meinert, Carolina Cruz-Neira
 *
 ****************** <VPR heading END do not edit this line> ******************/

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

#include <vpr/vprConfig.h>

#include <fstream>
#include <map>
#include <boost/utility.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <vpr/Util/Debug.h>
#include <vpr/Util/Assert.h>

#include <vpr/md/SIM/Network/NetworkGraph.h>


namespace vpr
{

namespace sim
{

// Crummy helper for crappy C++ I/O.
static void skipToEOL(std::istream& stream)
{
   char c;

   while ( (c = stream.get()) != '\n' )
   {
      /* Loop. */ ;
   }
}

vpr::ReturnStatus NetworkGraph::construct(const std::string& path)
{
   vpr::ReturnStatus status;
   std::ifstream input_file(path.c_str());

   if ( input_file.is_open() )
   {
      vpr::Uint32 node_count, full_edge_count, half_edge_count;
      std::map<int, boost::graph_traits<net_graph_t>::vertex_descriptor> vertex_map;
      std::string temp_str;
      vpr::Uint32 index, last_index, localhost_index;
      vpr::Uint16 node_type;

      input_file >> node_count;

      for ( vpr::Uint32 i = 0; i < node_count; ++i )
      {
         std::string node_ip;

         input_file >> index >> node_type >> node_ip;
         skipToEOL(input_file);

         // Now create the vertex and add it to the graph.
         NetworkNodePtr node_prop( new NetworkNode(index, node_type, node_ip) );
         vertex_map[index] = boost::add_vertex(NodeProperty( node_prop),
                                               mGraph);
      }

      // Add a node to the graph that will represent localhost.  It will have
      // an edge to the last node added to the graph.
      // index and node_type will have the values of the last vertex added to
      // the graph.  We will use this vertex to represent localhost.
      last_index      = index;
      localhost_index = last_index + 1;
      NetworkNodePtr node_prop(new NetworkNode(localhost_index, node_type,
                                               std::string("127.0.0.1")));
      vprASSERT(vertex_map.count(localhost_index) == 0 && "The index for localhost is already in use");
      vertex_map[localhost_index] = boost::add_vertex(NodeProperty(node_prop),
                                                      mGraph);

      input_file >> full_edge_count;

      // The file format tells us that there are twice as many edges as
      // actually listed--probably to indicate bidirectional links.
      half_edge_count = full_edge_count / 2;

      net_edge_t new_edge;
      bool added;
      boost::property_map<net_graph_t, boost::edge_weight_t>::type weight_map;
      weight_map = boost::get(boost::edge_weight_t(), mGraph);

      // This is outside the loop so that it can be used when making the
      // loopback link for localhost.
      vpr::Uint16 net_id;

      for ( vpr::Uint32 i = 0; i < full_edge_count; ++i )
      {
         double length, delay, bw;
         vpr::Uint32 from_node, to_node;
         std::string net_type, net_ip;

         input_file >> from_node >> to_node >> length >> delay >> bw
                    >> net_type >> net_id >> net_ip;

         // This is here mainly for debugging this clunky "parser".
         vprDEBUG(vprDBG_ALL, vprDBG_HVERB_LVL)
            << "Loading edge #" << i << ": (" << from_node << " --> "
            << to_node << ", " << length << " miles, " << delay << " us, "
            << bw << " Mbps, type: " << net_type << ", ID: " << net_id << ", "
            << net_ip << ")\n" << vprDEBUG_FLUSH;

         // Now add the edge to the graph.
         NetworkLine line(length, bw, delay, net_type, net_id, net_ip);
         boost::tie(new_edge, added) = boost::add_edge(vertex_map[from_node],
                                                       vertex_map[to_node],
                                                       LineProperty(line),
                                                       mGraph);

         if ( added )
         {
            weight_map[new_edge] = line.getWeight();
         }

         // Sanity check to ensure that we do not get stuck trying to read
         // bytes that aren't there.  This probably slows things down, but I
         // don't know of a better way to do this.
         if ( input_file.peek() == EOF && i + 1 < full_edge_count )
         {
            vprDEBUG(vprDBG_ALL, vprDBG_WARNING_LVL)
               << "WARNING: Expected " << full_edge_count
               << " edges, found " << i + 1 << std::endl << vprDEBUG_FLUSH;
            break;
         }
      }

      // Make a very, very short, very, very high-bandwidth link for the
      // localhost loopback.
      NetworkLine line(0.0000001f, 100000, 0.0000001f, std::string("LOOPBACK"),
                       net_id, std::string("127.0.0.0"));
      boost::tie(new_edge, added) = boost::add_edge(vertex_map[localhost_index],
                                                    vertex_map[last_index],
                                                    LineProperty(line),
                                                    mGraph);
      vprASSERT(added && "Addition of loopback line for localhost failed");
      weight_map[new_edge] = line.getWeight();

      mGraphValid = true;
   }
   else
   {
      vprDEBUG(vprDBG_ALL, vprDBG_CRITICAL_LVL)
         << "ERROR: Failed to open graph input file named " << path
         << std::endl << vprDEBUG_FLUSH;
      status.setCode(vpr::ReturnStatus::Fail);
   }

   return status;
}

vpr::sim::NetworkLine& NetworkGraph::getLineProperty(const NetworkGraph::net_edge_t& e)
{
   // XXX: Need to make an assertion that e is in mGraph!
//   vprASSERT(... && "Given edge not found in the graph!");

   boost::property_map<net_graph_t, network_line_t>::type line_prop_map;

   line_prop_map = boost::get(network_line_t(), mGraph);

   return line_prop_map[e];
}

void NetworkGraph::setLineProperty(const NetworkGraph::net_edge_t& e,
                                   const vpr::sim::NetworkLine& prop)
{
   // XXX: Need to make an assertion that e is in mGraph!
//   vprASSERT(... && "Given edge not found in the graph!");

   boost::property_map<net_graph_t, network_line_t>::type line_prop_map;

   line_prop_map    = boost::get(network_line_t(), mGraph);
   line_prop_map[e] = prop;
}

vpr::ReturnStatus NetworkGraph::getNodeWithAddr(const vpr::Uint32 addr,
                                                NetworkGraph::net_vertex_t& node)
{
   vprASSERT(isValid() && "Trying to use invalid graph");

   vpr::ReturnStatus status(vpr::ReturnStatus::Fail);
   boost::graph_traits<net_graph_t>::vertex_iterator vi, vi_end;
   NetworkNodePtr node_prop;

   boost::tie(vi, vi_end) = boost::vertices(mGraph);

   for ( ; vi != vi_end; ++vi )
   {
      node_prop = getNodeProperty(*vi);

      if ( node_prop->getIpAddress() == addr )
      {
         node = *vi;
         status.setCode(vpr::ReturnStatus::Succeed);
         break;
      }
   }

   return status;
}

vpr::sim::NetworkNodePtr NetworkGraph::getNodeProperty(const NetworkGraph::net_vertex_t& v)
{
   // XXX: Need to make an assertion that v is in mGraph!
//   vprASSERT(... && "Given vertex not found in the graph!");

   boost::property_map<net_graph_t, network_node_t>::type node_prop_map;

   node_prop_map = boost::get(network_node_t(), mGraph);

   return node_prop_map[v];
}

void NetworkGraph::setNodeProperty(const NetworkGraph::net_vertex_t& v,
                                   vpr::sim::NetworkNodePtr prop)
{
   // XXX: Need to make an assertion that v is in mGraph!
//   vprASSERT(... && "Given vertex not found in the graph!");

   boost::property_map<net_graph_t, network_node_t>::type node_prop_map;

   node_prop_map    = boost::get(network_node_t(), mGraph);
   node_prop_map[v] = prop;
}

NetworkGraph::VertexListPtr NetworkGraph::getShortestPath(const NetworkGraph::net_vertex_t& src,
                                                          const NetworkGraph::net_vertex_t& dest)
   const
{
   NetworkGraph::VertexListPtr vlist(new NetworkGraph::VertexList);
   std::vector<int> dist(boost::num_vertices(mGraph));
   std::vector<net_vertex_t> pred(boost::num_vertices(mGraph));
   NetworkGraph::net_vertex_t p(dest), q;
   boost::property_map<net_graph_t, network_node_t>::const_type node_prop_map;
   boost::property_map<net_graph_t, boost::edge_weight_t>::const_type weight_map;

   // NOTE: There is no default constructor for this type.  The copy
   // constructor must be used.
   boost::property_map<net_graph_t, boost::vertex_index_t>::const_type vertex_map
      = boost::get(boost::vertex_index_t(), mGraph);

   node_prop_map = boost::get(network_node_t(), mGraph);
   weight_map    = boost::get(boost::edge_weight_t(), mGraph);

   vprASSERT(boost::out_degree(src, mGraph) > 0 && "No outgoing edges from source");
   vprASSERT(boost::out_degree(dest, mGraph) > 0 && "No outgoing edges from destination");

   vprDEBUG_BEGIN(vprDBG_ALL, vprDBG_STATE_LVL)
      << "NetworkGraph::getShortestPath(): Looking up the shortest path "
      << "between " << node_prop_map[src]->getIpAddressString() << " and "
      << node_prop_map[dest]->getIpAddressString() << "\n" << vprDEBUG_FLUSH;

//   boost::dijkstra_shortest_paths(mGraph, src,
//                                  boost::distance_map(&dist[0]).predecessor_map(&pred[0]));
   boost::dijkstra_shortest_paths(mGraph, src, &pred[0], &dist[0], weight_map,
                                  vertex_map, std::less<int>(),
                                  boost::closed_plus<int>(),
                                  std::numeric_limits<int>::max(), 0,
                                  boost::default_dijkstra_visitor());

   vlist->push_back(dest);

   vprDEBUG(vprDBG_ALL, vprDBG_VERB_LVL)
      << "Path (dest <-- src): " << node_prop_map[dest]->getIpAddressString()
      << vprDEBUG_FLUSH;

   // Put the vertices returned in the predecessor map into a stack so that
   // we can reverse the order and put them into vlist.
   while ( (q = pred[p]) != p )
   {
      vprDEBUG_CONT(vprDBG_ALL, vprDBG_VERB_LVL)
         << " <-- " << node_prop_map[q]->getIpAddressString()
         << vprDEBUG_FLUSH;

      vlist->push_back(q);
      p = q;
   }

   vprDEBUG_CONT_END(vprDBG_ALL, vprDBG_STATE_LVL) << "\n" << vprDEBUG_FLUSH;
   vprASSERT(p == src && "Destination is unreachable from source!");

   std::reverse(vlist->begin(), vlist->end());

   return vlist;
}

NetworkGraph::VertexListPtr NetworkGraph::reversePath(NetworkGraph::VertexListPtr path)
{
   VertexListPtr new_path(new NetworkGraph::VertexList(path->size()));

   for ( vpr::Uint32 i = path->size(), j = 0; i > 0; i--, ++j )
   {
      (*new_path)[j] = (*path)[i - 1];
   }

   return new_path;
}

vpr::ReturnStatus NetworkGraph::getAllAddresses(NetworkGraph::AddressList& list)
{
   vpr::ReturnStatus status;
   boost::graph_traits<net_graph_t>::vertex_iterator vi, vi_end;
   NetworkNodePtr node_prop;

   boost::tie(vi, vi_end) = boost::vertices(mGraph);

   for ( ; vi != vi_end; ++vi )
   {
      node_prop = getNodeProperty(*vi);
      list.push_back(std::pair<net_vertex_t, vpr::Uint32>(*vi,
                                                          node_prop->getIpAddress()));
   }

   return status;
}

} // End of sim namespace

} // End of vpr namespace
