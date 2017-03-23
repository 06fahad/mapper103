/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3Mod.h
 * Author: rahma314
 *
 * Created on March 13, 2017, 3:49 PM
 */
using namespace std;
#include <vector>
#ifndef NODE_H
#define NODE_H

class Node {
public:
    unsigned nodeID; //ID of the intersection
    int reachingEdge; //ID of street segment used to reach this node
    vector<unsigned> edges_connected_to_node; //street segments connected to this node
    vector<unsigned> adjacent_nodes; //adjacent intersections connected to this intersection
    bool visited;
    double pathLength;

    //constructor for node
   Node();
    Node(unsigned _nodeID, vector<unsigned>street_segments_connected_to_node, vector<unsigned> adjacent_intersections);
    
    ~Node();
};



#endif /* NODE_H */

