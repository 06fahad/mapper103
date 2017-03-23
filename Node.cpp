/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Node.h"
#include <climits>
Node::Node() {
    ;
}
Node::Node(unsigned _nodeID, vector<unsigned>street_segments_connected_to_node, vector<unsigned> adjacent_intersections) {
    nodeID = _nodeID;
    visited = false;
    reachingEdge = -2;
    edges_connected_to_node = street_segments_connected_to_node;
    adjacent_nodes = adjacent_intersections;
    pathLength = INT_MAX;
}
Node::~Node() {
    edges_connected_to_node.clear();
    adjacent_nodes.clear();
}