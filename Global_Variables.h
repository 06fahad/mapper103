/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Global_Variables.h
 * Author: zhan2860
 *
 * Created on March 15, 2017, 1:03 AM
 */
#include <unordered_set>
#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

extern t_point rubber_pt; // Last point to which we rubber-banded.
extern std::vector<t_point> line_pts; // Stores the points entered by user clicks.

extern string mapName;
extern string mapPath;
extern string dbPath;

extern string keyboardBuff; //Temp storage for input.

extern float recentClickX, recentClickY;

extern bool findTextOff;

extern bool SubwaysOn;
extern bool POIsOn;
extern bool GreenOn;
extern bool WaterOn;
extern bool BuildingOn;

extern bool findButtonClicked;

extern bool drawIntersectionData;
extern bool intersectionClicked;

extern bool POIHovered;
extern bool userInputing;

extern double XStretchFactor;
extern double YStretchFactor;
extern double currentMouseX, currentMouseY;

extern vector <double> xyCoordsOfIntersection;

extern float km_per_char;
//vector <intersection_data> intersections; // vector containing intersection_data { LatLon position; string name;}

extern t_color orangeColor;
extern t_color yellowColor;

extern double average_lat; //average latitude for the map
extern vector <double> coords; //Coordinates for visible world

/*------------------------------------------------------------*/
//Other variables
extern bool endProgram; //finding the end of program

extern unsigned numOfStreetSegments;
extern unsigned numOfIntersections;
extern unsigned numOfPOIs;
extern unsigned numOfFeatures;
extern string mapNameX;
extern string mapPath;
extern string dbPath;
extern unsigned numOfStreets;

//3-D R-tree vector with layers of intersections, features, POI 
//The R-tree has(x,y,LOD) as it's element
extern double LatAve;

//Global Vectors of vectors

extern vector<vector<LatLon> > street_seg_curve_point; //Stores curve point positions by segment index
extern vector<vector<unsigned> > street_street_segments; //Vector Storing the street segments under their respective street index
extern vector<vector<unsigned> > street_all_intersections; //Vector storing intersections under their respective street index
extern vector<vector<unsigned> > street_segments_for_intersection_id; //CONTAINS STREET SEGMENTS CONNECTED TO EVERY INTERSECTION

//Global Vectors
extern vector<LatLon> POI_LatLon; //Vector containing the latlons of the POIS
extern vector<LatLon> Intersection_LatLon; //CONTAINS THE LATLON INFO OF EACH POI ID CORRESPONDING TO THE INDEX OF THE OUTER VECTOR
extern vector<string> street_names_by_index; //Vector of street indexs and their names

//vector<vector<unsigned>> intersection_hash_table;
//vector<vector<unsigned>> interest_hash_table;

extern vector<unsigned> street_seg_count; //CONTAINS THE NUMBER OF STREET SEGS CONNECTING TO EACH INTERSECTION    
extern vector<unsigned>street_seg_curve_point_num; //Stores number of curve points for respective street segments 
extern vector<double>street_seg_length; //Contains the length of every street segment
extern vector<double>street_seg_travel_time; //Vector containing the travel times for every street segment by index

extern vector<POIdata> POI_Data; //Vector Containing the XY positions of all POIs
extern vector<interXY> intersection_XY; //Vector containing all intersection XY coordinates
extern vector<featureData> feature_data; //Vector containing all the feature bounds in XY coords

extern vector<int> street_segment_LOD; // LOD for each street segment by index

extern multimap<string, unsigned> hash_streetID; //MULTIMAP
extern multimap<string, unsigned> hash_POIID; 

extern vector <OSMID> motorways;

//to use the following vector: street_seg_info[streetSegmentID]. This has the same functionality as getStreetSegmentInfo(streetID).
//Example: street_seg_info[10000].from. This returns the "from" info of the street segment with the ID 10000
extern vector<StreetSegmentInfo> street_seg_info; //vector containing street segment info for streetSegmentID corresponding to the index    

extern unordered_map <OSMID, OSMID> motorwayOSMID;
extern unordered_map <OSMID, OSMID> motorwayLinkOSMID;
extern unordered_map <OSMID, OSMID> trunkOSMID;
extern unordered_map <OSMID, OSMID> trunkLinkOSMID;
extern unordered_map <OSMID, OSMID> primaryOSMID;
extern unordered_map <OSMID, OSMID> secondaryOSMID;
extern unordered_map <OSMID, OSMID> tertiaryOSMID;
extern unordered_map <OSMID, OSMID> residentialOSMID;
extern unordered_map <OSMID, OSMID> unclassifiedOSMID;
extern unordered_map <OSMID, OSMID> roadsOSMID;

extern unordered_map <OSMID, LatLon> OSMID_LatLon_unordered_map;
extern unordered_map <unsigned, vector<OSMID>> wayIndex_OSMIDSubwayVector_unordered_map;

extern vector<Node> Nodes; //vector containing Node objects where the vector index corresponds to the intersectionID which is also the nodeID of the object

//Feature data structures
extern vector<unsigned> UnknownFeatures;
extern vector<unsigned> ParkFeatures;
extern vector<unsigned> BeacheFeatures;
extern vector<unsigned> LakeFeatures;
extern vector<unsigned> RiverFeatures;
extern vector<unsigned> IslandFeatures;
extern vector<unsigned> ShorelineFeatures;
extern vector<unsigned> BuildingFeatures;
extern vector<unsigned> GreenspaceFeatures;
extern vector<unsigned> GolfcourseFeatures;
extern vector<unsigned> StreamFeatures;

extern vector<featureData> PolyFeaturesByArea;

extern bool initialLoad;

extern double instanceTravelTime;
extern double currentFastestTravelTime;
extern unsigned closestDestIterPOI;
extern unsigned instanceDestIterPOI;

extern double maxSpeed;
extern unordered_map<string, vector<unsigned>> POIName_Index_Map;
extern unordered_map<string, unordered_set<unsigned>> POI_closest_intersection_Map;
extern unsigned startNodeID;
extern unsigned endNodeID;


extern bool showOptions;
extern bool directionInput;

#endif /* GLOBAL_VARIABLES_H */

