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

    //    if (srcNode->visited == true)cout << "Source Node is: TRUE" << endl;
    //    else cout << "Source Node is: FALSE " << endl;
    //
    //    if (found) {
    //        cout << "Found!" << endl;
    //    }
    //
    //    else {
    //        cout << ":(" << endl;
    //    }



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



/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

//// Returns the shortest travel time path (vector of street segments) from 
//// the start intersection to a point of interest with the specified name.
//// The path will begin at the specified intersection, and end on the 
//// intersection that is closest (in Euclidean distance) to the point of 
//// interest.
//// If no such path exists, returns an empty (size == 0) vector.
////THIS ONE IS BY EUCLIDION DISTANCE
//
//std::vector<unsigned> find_path_to_point_of_interest(const unsigned intersect_id_start,
//        const std::string point_of_interest_name,
//        const double turn_penalty) {
//
//    //Getting the POI indexes with that name
//    vector<unsigned> POIIndices = find_POI_ids_from_name(point_of_interest_name);
//    currentFastestTravelTime = INT_MAX;
//    unsigned secondClosest = INT_MAX;
//    unsigned thridClosest = INT_MAX;
//
//    for (unsigned i = 0; i < POIIndices.size(); i++) {
//
//        //Closest Intersection to this POI
//        unsigned closestInt = find_closest_intersection(POI_LatLon[POIIndices[i]]);
//
//        instanceTravelTime = find_distance_between_two_points(POI_LatLon[POIIndices[i]], Intersection_LatLon[closestInt]);
//
//        if (instanceTravelTime < currentFastestTravelTime) {
//            thridClosest = secondClosest;
//            secondClosest = closestDestIterPOI;
//            closestDestIterPOI = closestInt;
//            currentFastestTravelTime = instanceTravelTime;
//        }
//
//    }
//
//    vector<unsigned> closestPOIPath = find_path_between_intersections(intersect_id_start, closestDestIterPOI, turn_penalty);
//    vector<unsigned> secondclosestPOIPath;
//    vector<unsigned> thirdclosestPOIPath;
//    
//    double maxTime1 = compute_path_travel_time(closestPOIPath, turn_penalty);
//    double maxTime2 = INT_MAX;
//    double maxTime3 = INT_MAX;
//
//    if (secondClosest != INT_MAX) {
//        secondclosestPOIPath = find_path_between_intersections(intersect_id_start, secondClosest, turn_penalty);
//        maxTime2 = compute_path_travel_time(secondclosestPOIPath, turn_penalty);
//    }else 
//
//    if (thridClosest != INT_MAX) {
//        thirdclosestPOIPath = find_path_between_intersections(intersect_id_start, thridClosest, turn_penalty);
//        maxTime3 = compute_path_travel_time(thirdclosestPOIPath, turn_penalty);
//    }
//
//
//
//
//
//    //        cout << endl << "Source Node: " << intersect_id_start << endl;
//    //        cout << "Destination Node: " << closestDestIterPOI << endl;
//    //        cout<<"what we get: "<<maxTime<<endl;
//    if (maxTime1 > maxTime2 && maxTime3 > maxTime2) return secondclosestPOIPath;
//    else if (maxTime1 > maxTime3 && maxTime2 > maxTime3) return thirdclosestPOIPath;
//    else return closestPOIPath;
//}

void draw_direction_list(std::vector<unsigned> SegsID) {
    settextrotation(0);
    //set up the vector for name, distance, and travel time
    int count = -1;
    set_coordinate_system(GL_SCREEN);
    //unsigned streetIndex;
    string name, printDistance, printTimeMin, printTimeSec;
    string Message;
    double travelTime = 0;
    double distance = 0;
    vector<string> namelist;
    vector<double> travelTimeList;
    vector<double> distanceList;
    vector<int> turnDirection;
    const int x_width = 350;
    const int y_top = 0;

    const double fontWidth = 1.47 / 2;
    int font = 12;
    int xLeft = 15;
    int yTop = y_top + 15 + font / 2;
    unsigned numOfChar = floor(x_width / (font * fontWidth)) + 6;



    string streetName, partialString, Direction;
    vector<string> parsedString;

    setcolor(255, 255, 255, 240);
    fillrect(0, y_top, x_width, 1800);
    setfontsize(font + 8);
    drawtext(x_width / 2, yTop, "Directions", 500, 800);
    yTop = yTop + (font + 8) / 2 + 5;
    setfontsize(font);
    cout << "Count: " << count << endl;
    cout << SegsID.size() << endl;

    while (count< int(SegsID.size())) {

        cout << "in Loop" << endl;
        travelTime = 0;
        distance = 0;
        do {
            count++;
            distance = distance + find_street_segment_length(SegsID[count]) / 1000; //result in km
            travelTime = travelTime + find_street_segment_travel_time(SegsID[count]) / 60; //result in mins


        } while (count<int(SegsID.size()) - 1 && street_seg_info[SegsID[count]].streetID == street_seg_info[SegsID[count + 1]].streetID);
        name = street_names_by_index[street_seg_info[SegsID[count]].streetID];
        travelTimeList.push_back(travelTime);
        distanceList.push_back(distance);
        namelist.push_back(name);
        if (count<int(SegsID.size() - 1))
            turnDirection.push_back(turn_direction(SegsID[count], SegsID[count + 1]));
        if (count == int(SegsID.size()) - 1)
            break;
    }

    //set up the box at bottom left to show all the name, distance, time require to travel
    setcolor(0, 0, 0);
    for (unsigned i = 0; i < namelist.size(); i++) {
        streetName = namelist[i];
        if (streetName.size() < numOfChar) {
            parsedString.push_back(streetName);
        } else {
            while (streetName.size() > numOfChar) {
                partialString = streetName.substr(0, numOfChar);
                streetName.erase(0, numOfChar);
                parsedString.push_back(partialString);
            }
            parsedString.push_back(streetName);
        }
        for (unsigned j = 0; j < parsedString.size(); j++) {
            drawtextleft(xLeft, yTop, parsedString[j], x_width, 1800);
            yTop += font + 5;
        }
        printDistance = to_string(distanceList[i]);
        if (log10(distanceList[i]) < 0) printDistance.resize(4);
        else printDistance.resize(4 + int(log10(distanceList[i]))); //in km
        printTimeMin = to_string(int(travelTimeList[i]));
        printTimeSec = to_string((travelTimeList[i] - floor(travelTimeList[i]))*60);
        if ((travelTimeList[i] - floor(travelTimeList[i]))*60 < 10) printTimeSec.resize(1);
        else printTimeSec.resize(2);
        switch (turnDirection[i]) {
            case 1:
            {
                Direction = "Turn right";
                break;
            }
            case 2:
            {
                Direction = "Turn left";
                break;
            }
            case 3:
            {
                Direction = "Go straight";
                break;
            }
            case 4:
            {
                Direction = "Make a legal U turn";
                break;
            }
            default:
            {
                Direction = "You reach destination";
                cout << "default case";
                break;
            }

        }
        parsedString.clear();
        Message = Direction + " in " + printDistance + " km  ";
        if (Message.size() < numOfChar)
            parsedString.push_back(Message);
        else {
            while (Message.size() > numOfChar) {
                partialString = Message.substr(0, numOfChar);
                Message.erase(0, numOfChar);
                parsedString.push_back(partialString);
            }
            parsedString.push_back(Message);
        }

        for (unsigned j = 0; j < parsedString.size(); j++) {
            drawtextleft(xLeft, yTop, parsedString[j], x_width, 1800);
            yTop += font + 5;
        }

        parsedString.clear();
        Message = "Estimated travel time " + printTimeMin + " minutes " + printTimeSec + " seconds";
        if (Message.size() < numOfChar)
            parsedString.push_back(Message);
        else {
            while (Message.size() > numOfChar) {
                partialString = Message.substr(0, numOfChar);
                Message.erase(0, numOfChar);
                parsedString.push_back(partialString);
            }
            parsedString.push_back(Message);
        }

        for (unsigned j = 0; j < parsedString.size(); j++) {
            drawtextleft(xLeft, yTop, parsedString[j], x_width, 1800);
            yTop += font + 5;
        }
        yTop += 5;
        parsedString.clear();
    }
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


//

float find_angle_between_point_in_degree(float deltaX, float deltaY) {
    float rotation;
    if (abs(deltaX) < 0.00000001) {
        if (deltaY > 0)
            rotation = 90;
        else
            rotation = -90;



    } else {//if delta x is smaller than 10e-8 and when the scale of we are using is around 10e-4, we may overflow atan which takes in a double
        rotation = atan(deltaY / deltaX);
        rotation = -rotation * 180 / PI;
        if (deltaX < 0) {
            if (rotation > 0)
                rotation -= 180;
            else
                rotation += 180;
        }


    }
    return rotation;
}

int turn_direction(unsigned segIDfrom, unsigned segIDto) {

    unsigned segIDfromfrom, segIDfromto, segIDtofrom, segIDtoto, midpoint;
    t_point first, second, third;
    float deltaX, deltaY, angle1, angle2;
    bool is1Curve, is2Curve;
    segIDfromfrom = street_seg_info[segIDfrom].from;
    segIDfromto = street_seg_info[segIDfrom].to;
    segIDtofrom = street_seg_info[segIDto].from;
    segIDtoto = street_seg_info[segIDto].to;

    if (street_seg_info[segIDfrom].curvePointCount == 0)
        is1Curve = false;
    else
        is1Curve = true;
    if (street_seg_info[segIDto].curvePointCount == 0)
        is2Curve = false;
    else
        is2Curve = true;

    Intersection_LatLon[0];

    if (segIDfromfrom == segIDtofrom) {//1
        midpoint = segIDfromfrom;

        if (!is1Curve)
            first = LatLon_to_t_point(Intersection_LatLon[segIDfromto]);
        else
            first = LatLon_to_t_point(street_seg_curve_point[segIDfrom][0]);

        second = LatLon_to_t_point(Intersection_LatLon[midpoint]);

        if (!is2Curve)
            third = LatLon_to_t_point(Intersection_LatLon[segIDtoto]);
        else
            third = LatLon_to_t_point(street_seg_curve_point[segIDto][0]);


    } else if (segIDfromfrom == segIDtoto) {//2
        midpoint = segIDfromfrom;

        if (!is1Curve)
            first = LatLon_to_t_point(Intersection_LatLon[segIDfromto]);
        else
            first = LatLon_to_t_point(street_seg_curve_point[segIDfrom][0]);

        second = LatLon_to_t_point(Intersection_LatLon[midpoint]);

        if (!is2Curve)
            third = LatLon_to_t_point(Intersection_LatLon[segIDtofrom]);
        else
            third = LatLon_to_t_point(street_seg_curve_point[segIDto][street_seg_info[segIDto].curvePointCount - 1]);
    } else if (segIDfromto == segIDtofrom) {//3
        midpoint = segIDfromto;

        if (!is1Curve)
            first = LatLon_to_t_point(Intersection_LatLon[segIDfromfrom]);
        else
            first = LatLon_to_t_point(street_seg_curve_point[segIDfrom][street_seg_info[segIDfrom].curvePointCount - 1]);

        second = LatLon_to_t_point(Intersection_LatLon[midpoint]);

        if (!is2Curve)
            third = LatLon_to_t_point(Intersection_LatLon[segIDtoto]);
        else
            third = LatLon_to_t_point(street_seg_curve_point[segIDto][0]);
    } else if (segIDfromto == segIDtoto) {//4
        midpoint = segIDfromto;

        if (!is1Curve)
            first = LatLon_to_t_point(Intersection_LatLon[segIDfromfrom]);
        else
            first = LatLon_to_t_point(street_seg_curve_point[segIDfrom][street_seg_info[segIDfrom].curvePointCount - 1]);

        second = LatLon_to_t_point(Intersection_LatLon[midpoint]);

        if (!is2Curve)
            third = LatLon_to_t_point(Intersection_LatLon[segIDtofrom]);
        else
            third = LatLon_to_t_point(street_seg_curve_point[segIDto][street_seg_info[segIDto].curvePointCount - 1]);
    }

    deltaX = second.x - first.x;
    deltaY = second.y - first.y;
    angle1 = find_angle_between_point_in_degree(deltaX, deltaY);
    deltaX = third.x - second.x;
    deltaY = third.y - second.y;
    angle2 = find_angle_between_point_in_degree(deltaX, deltaY);

    if (abs(angle2 - angle1) < 10)
        return 3; //go straight
    else if (abs(angle2 - angle1) > 170)
        return 4; // U turn
    else if (angle2 - angle1 >= 10)
        return 2; //left
    else if (angle2 - angle1 <= -10)
        return 1; //right
    else {
        cout << "turn direction malfunctioning-unexpected error " << endl;
        return 0;
    }

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
