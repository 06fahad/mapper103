/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "m2.h"
#include "m3.h"
#include "m1.h"
#include "m2Mod.h"
#include "m3Mod.h"
#include "DataStructures.h"
#include "Global_Variables.h"
#include <queue>
#include <climits>
#include<unordered_set>
using namespace std;

struct waveElement {
    Node* node;
    int reachingEdgeID; //street segment used to reach this node
    double reachingTime;

    waveElement(Node* _node, int ID, double time) {
        node = _node;
        reachingEdgeID = ID;
        reachingTime = time;
    }

    bool operator<(const waveElement &g) const {
        return reachingTime > g.reachingTime;
    }
};

double instanceTravelTime;
double currentFastestTravelTime;
unsigned closestDestIterPOI;
unsigned instanceDestIterPOI;
unsigned startNodeID;
unsigned endNodeID; //need this for POI function
#define NO_EDGE -1

bool bfsPathFound(Node* srcNode, Node* destNode, double turn_penalty);
list<int>bfsTrackBack(unsigned destID);
// Returns the time required to travel along the path specified, in seconds. 
// The path is given as a vector of street segment ids, and this function 
// can assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus the given turn_penalty (in seconds) per turn implied by the path. 
// A turn occurs when two consecutive street segments have different street IDs.

double compute_path_travel_time(const std::vector<unsigned>& path,
        const double turn_penalty) {
    double travelTime = 0;
    if (path.size() != 0) {

        travelTime = street_seg_travel_time[path[0]];
        //    unsigned size = street_seg_travel_time.size();
        //    cout << "size: " << size << endl;
        for (unsigned i = 0; i < path.size(); i++) {
            if (i >= 1 && street_seg_info[path[i]].streetID == street_seg_info[path[i - 1]].streetID) {
                travelTime += street_seg_travel_time[path[i]];
            } else if (i >= 1 && street_seg_info[path[i]].streetID != street_seg_info[path[i - 1]].streetID) {
                travelTime += street_seg_travel_time[path[i]] + turn_penalty;
            }
        }

    }
    //   cout << "travelTime: " << travelTime << endl;
    return travelTime;
}



// Returns a path (route) between the start intersection and the end 
// intersection, if one exists. This routine should return the shortest path
// between the given intersections when the time penalty to turn (change
// street IDs) is given by turn_penalty (in seconds).
// If no path exists, this routine returns an empty (size == 0) vector. 
// If more than one path exists, the path with the shortest travel time is 
// returned. The path is returned as a vector of street segment ids; traversing 
// these street segments, in the returned order, would take one from the start 
// to the end intersection.

std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start,
        const unsigned intersect_id_end,
        const double turn_penalty) {


    // bool found = false;
    list<int>name;
    startNodeID = intersect_id_start;
    endNodeID = intersect_id_end;
    vector <unsigned> path; //path to return 

    Node* srcNode = &Nodes [intersect_id_start]; //get source Node

    //    cout <<"intersect_id_start: "<<intersect_id_start<<", srcNode: "<<srcNode->nodeID<<endl;
    //    srcNode->nodeID = 69;
    //    cout <<"srcNode after: "<<Nodes[intersect_id_start].nodeID<<endl;


    srcNode->pathLength = 0; //set pathLength on starting node = 0
    srcNode->reachingEdge = NO_EDGE; //no reaching edge for source node

    Node* destNode = &Nodes [intersect_id_end]; // get destination node

    bool found = bfsPathFound(srcNode, destNode, turn_penalty); //pass it into bfsPathFound to find path




    if (found) name = bfsTrackBack(intersect_id_end);

    for (auto iterator = name.begin(); iterator != name.end(); iterator++) {
        path.push_back(*iterator);
    }

    return path;
}

bool bfsPathFound(Node* currNode, Node* destNode, double turn_penalty) {


    bool found = false;
    priority_queue<waveElement> waveFront;
    waveFront.push(waveElement(currNode, NO_EDGE, 0));
    Node* connectingNode;
    vector<Node*> turnBackFalse;

    while (!waveFront.empty()) {

        waveElement currWave = waveFront.top();
        waveFront.pop();
        currWave.node->visited = true;
        turnBackFalse.push_back(currWave.node);



        for (unsigned i = 0; i < currWave.node->edges_connected_to_node.size(); i++) { //go through all the edges connected through node
            double travel_time = 0;

            if (street_seg_info[currWave.node->edges_connected_to_node[i]].oneWay != true) { //if street segment is not one way
                if (street_seg_info[currWave.node->edges_connected_to_node[i]].from == currWave.node->nodeID) { //if currNode is not the from of the street segment
                    //cout<<"First if"<<endl;
                    connectingNode = &Nodes[street_seg_info[currWave.node->edges_connected_to_node[i]].to]; //then next node to visit is the from of the street segment
                } else if (street_seg_info[currWave.node->edges_connected_to_node[i]].to == currWave.node->nodeID) { //if currNode is not the to of the street segment
                    //cout <<"second if"<<endl;
                    connectingNode = &Nodes[street_seg_info[currWave.node->edges_connected_to_node[i]].from]; //then next node to visit is the to of the street segment
                } else {
                    //cout << "something unexpected happened in bfsPathFound" << endl;
                }

                //if currNode is not the source and the currNode reaching edge's streetID 
                //is not the same as the streetID of the street segment i am looking at at this moment
                if (currWave.reachingEdgeID != NO_EDGE && street_seg_info[currWave.node->edges_connected_to_node[i]].streetID != street_seg_info[currWave.node->reachingEdge].streetID) {
                    travel_time += turn_penalty; //add turn penalty to path                                                                                                                                        
                }
                travel_time += street_seg_travel_time[currWave.node->edges_connected_to_node[i]] + currWave.reachingTime; //add existing weight on node + travel time of segment

                waveElement nextWave = waveElement(connectingNode, currWave.node->edges_connected_to_node[i], travel_time);

                if (nextWave.reachingTime < connectingNode->pathLength && connectingNode->visited == false) {
                    waveFront.push(nextWave);
                    Nodes[connectingNode->nodeID].reachingEdge = nextWave.reachingEdgeID;
                    Nodes[connectingNode->nodeID].pathLength = nextWave.reachingTime;
                    if (connectingNode->nodeID == destNode->nodeID) {
                        found = true;

                    }

                }
            } else if (street_seg_info[currWave.node->edges_connected_to_node[i]].oneWay == true
                    && street_seg_info[currWave.node->edges_connected_to_node[i]].from == currWave.node->nodeID) { //if it is one way and going away from current node
                connectingNode = &Nodes[street_seg_info[currWave.node->edges_connected_to_node[i]].to];

                //if currNode is not the source and the currNode reaching edge's streetID 
                //is not the same as the streetID of the street segment i am looking at at this moment
                if (currWave.reachingEdgeID != NO_EDGE && street_seg_info[currWave.node->edges_connected_to_node[i]].streetID != street_seg_info[currWave.node->reachingEdge].streetID) {
                    travel_time += turn_penalty; //add turn penalty to path                                                                                                                                        
                }
                travel_time += street_seg_travel_time[currWave.node->edges_connected_to_node[i]] + currWave.reachingTime; //add existing weight on node + travel time of segment

                waveElement nextWave = waveElement(connectingNode, currWave.node->edges_connected_to_node[i], travel_time);

                if (nextWave.reachingTime < connectingNode->pathLength && connectingNode->visited == false) {
                    waveFront.push(nextWave);
                    Nodes[connectingNode->nodeID].reachingEdge = nextWave.reachingEdgeID;
                    Nodes[connectingNode->nodeID].pathLength = nextWave.reachingTime;
                    if (connectingNode->nodeID == destNode->nodeID) {
                        found = true;

                    }

                }
            }



        }
    }

    //For the POI function
    instanceTravelTime = destNode->pathLength;
    instanceDestIterPOI = destNode->nodeID;


    //resetting visited flags

    for (unsigned i = 0; i < turnBackFalse.size(); i++) {
        turnBackFalse[i]->visited = false;
        turnBackFalse[i]->pathLength = INT_MAX;
    }


    turnBackFalse.clear();

    waveFront = priority_queue<waveElement>();

    return found;

}

list<int>bfsTrackBack(unsigned dest_ID) {
    list<int> path;

    Node *currNode = &Nodes[dest_ID];
    int prevEdge = currNode->reachingEdge;
    Node* temp;

    //    cout << "dest Node: " << currNode->nodeID << endl;
    while (currNode->reachingEdge != NO_EDGE) {
        temp = currNode;
        //cout <<i<<endl;
        path.push_front(prevEdge);
        if (street_seg_info[prevEdge].from == currNode->nodeID) {
            currNode = &Nodes[street_seg_info[prevEdge].to];
        } else if (street_seg_info[prevEdge].to == currNode->nodeID) {
            currNode = &Nodes[street_seg_info[prevEdge].from];
        } else {
            //           cout << "something went wrong in bfsTrackBack" << endl;
        }
        prevEdge = currNode->reachingEdge;

        temp->pathLength = INT_MAX;
        temp->reachingEdge = -2;
        temp->visited = false;

    }

    currNode->visited = false;
    currNode->pathLength = INT_MAX;
    currNode->reachingEdge = -2;

    return path;
}







//Returns a vector of POIs with the provided name
//returns a vector of length 0 if there are no vectors with that name

vector<unsigned> find_POI_ids_from_name(std::string POI_name) {

    auto POIIds = hash_POIID.equal_range(POI_name);

    vector<unsigned> POIsWithThisName;
    for (auto iter = POIIds.first; iter != POIIds.second; iter++) {
        POIsWithThisName.push_back(iter->second);
    }

    return POIsWithThisName;

}





//THIS ONE IS A HYBRID OF THE TWO ABOVE
// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// The path will begin at the specified intersection, and end on the 
// intersection that is closest (in Euclidean distance) to the point of 
// interest.
// If no such path exists, returns an empty (size == 0) vector.

std::vector<unsigned> find_path_to_point_of_interest(const unsigned intersect_id_start,
        const std::string point_of_interest_name,
        const double turn_penalty) {
    //resetNodes();

    vector <unsigned> path; //path to return 
    startNodeID = intersect_id_start;
    bool pathFound = find_path_POI_helper(intersect_id_start, point_of_interest_name, turn_penalty);

    list<int> pathToPOI;

    if (pathFound) {
        pathToPOI = bfsTrackBack(endNodeID);
        for (auto iterator = pathToPOI.begin(); iterator != pathToPOI.end(); iterator++) {
            path.push_back(*iterator);
        }
        pathToPOI.clear();
    }
   

    return path;

}



////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned find_path_POI_helper(unsigned srcID, string POI_Name, double turn_penalty) {


    unordered_set<unsigned> POI_closest_intersections = POI_closest_intersection_Map[POI_Name];

    //Data extraction was find here
    //    for (auto iter = POI_closest_intersections.begin(); iter != POI_closest_intersections.end(); iter++) {
    //        cout << iter.operator *() <<", " << endl;
    //    }

    Node* currNode = &Nodes[srcID];
    currNode->pathLength = 0; //set pathLength on starting node = 0
    currNode->reachingEdge = NO_EDGE; //no reaching edge for source node


    bool found = false;
    priority_queue<waveElement> waveFront;
    waveFront.push(waveElement(currNode, NO_EDGE, 0));
    //double bestTravelTime = 50000000; //best travel time is set to a really large number initially but will be replaced with the time on the destination node when it is found
    Node* connectingNode;

    //Reset tracker vector
    vector<Node*> turnBackFalse;

    while (!waveFront.empty()) {

        waveElement currWave = waveFront.top();
        waveFront.pop();
        currWave.node->visited = true;
        turnBackFalse.push_back(currWave.node);

        auto idCheck = POI_closest_intersections.find(currWave.node->nodeID);
        if (idCheck != POI_closest_intersections.end()) {
            found = true;
            endNodeID = currWave.node->nodeID;
            // cout << "End intersection: " << endNodeID << endl;
            for (unsigned c = 0; c < turnBackFalse.size(); c++) {
                turnBackFalse[c]->visited = false;
                turnBackFalse[c]->pathLength = INT_MAX;
            }


            turnBackFalse.clear();
            POI_closest_intersections.clear();
            waveFront = priority_queue<waveElement>();

            return true;;

        }


        for (unsigned i = 0; i < currWave.node->edges_connected_to_node.size(); i++) { //go through all the edges connected through node

            double travel_time = 0;

            if (street_seg_info[currWave.node->edges_connected_to_node[i]].oneWay != true) { //if street segment is not one way
                if (street_seg_info[currWave.node->edges_connected_to_node[i]].from == currWave.node->nodeID) { //if currNode is not the from of the street segment
                    //cout<<"First if"<<endl;
                    connectingNode = &Nodes[street_seg_info[currWave.node->edges_connected_to_node[i]].to]; //then next node to visit is the from of the street segment
                } else if (street_seg_info[currWave.node->edges_connected_to_node[i]].to == currWave.node->nodeID) { //if currNode is not the to of the street segment
                    //cout <<"second if"<<endl;
                    connectingNode = &Nodes[street_seg_info[currWave.node->edges_connected_to_node[i]].from]; //then next node to visit is the to of the street segment
                } else {
                    //cout << "something unexpected happened in bfsPathFound" << endl;
                }


                //if currNode is not the source and the currNode reaching edge's streetID 
                //is not the same as the streetID of the street segment i am looking at at this moment

                if (currWave.reachingEdgeID != NO_EDGE && street_seg_info[currWave.node->edges_connected_to_node[i]].streetID != street_seg_info[currWave.node->reachingEdge].streetID) {
                    travel_time += turn_penalty; //add turn penalty to path                                                                                                                                        
                }


                travel_time += street_seg_travel_time[currWave.node->edges_connected_to_node[i]] + currWave.reachingTime; //add existing weight on node + travel time of segment

                waveElement nextWave = waveElement(connectingNode, currWave.node->edges_connected_to_node[i], travel_time);

                if (nextWave.reachingTime < connectingNode->pathLength && connectingNode->visited == false) {
                    waveFront.push(nextWave);
                    Nodes[connectingNode->nodeID].reachingEdge = nextWave.reachingEdgeID;
                    Nodes[connectingNode->nodeID].pathLength = nextWave.reachingTime;
                    turnBackFalse.push_back(&Nodes[connectingNode->nodeID]);
                }
            } else if (street_seg_info[currWave.node->edges_connected_to_node[i]].oneWay == true
                    && street_seg_info[currWave.node->edges_connected_to_node[i]].from == currWave.node->nodeID) { //if it is one way and going away from current node
                connectingNode = &Nodes[street_seg_info[currWave.node->edges_connected_to_node[i]].to];

                //if currNode is not the source and the currNode reaching edge's streetID 
                //is not the same as the streetID of the street segment i am looking at at this moment
                if (currWave.reachingEdgeID != NO_EDGE && street_seg_info[currWave.node->edges_connected_to_node[i]].streetID != street_seg_info[currWave.node->reachingEdge].streetID) {
                    travel_time += turn_penalty; //add turn penalty to path                                                                                                                                        
                }
                travel_time += street_seg_travel_time[currWave.node->edges_connected_to_node[i]] + currWave.reachingTime; //add existing weight on node + travel time of segment

                waveElement nextWave = waveElement(connectingNode, currWave.node->edges_connected_to_node[i], travel_time);

                if (nextWave.reachingTime < connectingNode->pathLength && connectingNode->visited == false) {
                    waveFront.push(nextWave);
                    Nodes[connectingNode->nodeID].reachingEdge = nextWave.reachingEdgeID;
                    Nodes[connectingNode->nodeID].pathLength = nextWave.reachingTime;
                    turnBackFalse.push_back(&Nodes[connectingNode->nodeID]);
                }
            }
        }
    }


    //resetting visited flags

    for (unsigned i = 0; i < turnBackFalse.size(); i++) {
        turnBackFalse[i]->visited = false;
        turnBackFalse[i]->pathLength = INT_MAX;
    }


    turnBackFalse.clear();
    POI_closest_intersections.clear();
    waveFront = priority_queue<waveElement>();

    return found;

}
