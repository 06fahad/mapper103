
#include <iostream>
#include <functional>
#include <string>
#include "m2.h"
#include "m3.h"
#include "m1.h"
#include "m2Mod.h"
#include "m3Mod.h"
#include "Global_Variables.h"
#include "StreetsDatabaseAPI.h"
#include <unordered_set>

using namespace std;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::point<float, 3, bg::cs::cartesian> cartesian_LOD;
typedef std::pair<point, unsigned> value; // x coord ,y coord ,LOD
typedef std::pair<cartesian_LOD, unsigned> index_value;

////Global Variables
//int runCount = 0;
//

bool endProgram = false; //finding the end of program

unsigned numOfStreetSegments = 0;
unsigned numOfStreets = 0;
unsigned numOfIntersections = 0;
unsigned numOfPOIs = 0;
unsigned numOfFeatures = 0;

//3-D R-tree vector with layers of intersections, features, POI 
//The R-tree has(x,y,LOD) as it's element
double LatAve;

//Global Vectors of vectors

vector<vector<LatLon> > street_seg_curve_point; //Stores curve point positions by segment index
vector<vector<unsigned> > street_street_segments; //Vector Storing the street segments under their respective street index
vector<vector<unsigned> > street_all_intersections; //Vector storing intersections under their respective street index
vector<vector<unsigned> > street_segments_for_intersection_id; //CONTAINS STREET SEGMENTS CONNECTED TO EVERY INTERSECTION

//Global Vectors
vector<LatLon> POI_LatLon; //Vector containing the latlons of the POIS
vector<LatLon> Intersection_LatLon; //CONTAINS THE LATLON INFO OF EACH POI ID CORRESPONDING TO THE INDEX OF THE OUTER VECTOR
vector<string> street_names_by_index; //Vector of street indexs and their names

//vector<vector<unsigned>> intersection_hash_table;
//vector<vector<unsigned>> interest_hash_table;

vector<unsigned> street_seg_count; //CONTAINS THE NUMBER OF STREET SEGS CONNECTING TO EACH INTERSECTION    
vector<unsigned>street_seg_curve_point_num; //Stores number of curve points for respective street segments 
vector<double>street_seg_length; //Contains the length of every street segment
vector<double>street_seg_travel_time; //Vector containing the travel times for every street segment by index

vector<POIdata> POI_Data; //Vector Containing the XY positions of all POIs
vector<interXY> intersection_XY; //Vector containing all intersection XY coordinates
vector<featureData> feature_data; //Vector containing all the feature bounds in XY coords


vector<int> street_segment_LOD; // LOD for each street segment by index

multimap<string, unsigned> hash_streetID; //MULTIMAP
multimap<string, unsigned> hash_POIID; //Multimap for POI names

vector <OSMID> motorways;

//to use the following vector: street_seg_info[streetSegmentID]. This has the same functionality as getStreetSegmentInfo(streetID).
//Example: street_seg_info[10000].from. This returns the "from" info of the street segment with the ID 10000
vector<StreetSegmentInfo> street_seg_info; //vector containing street segment info for streetSegmentID corresponding to the index    

unordered_map <OSMID, OSMID> motorwayOSMID;
unordered_map <OSMID, OSMID> motorwayLinkOSMID;
unordered_map <OSMID, OSMID> trunkOSMID;
unordered_map <OSMID, OSMID> trunkLinkOSMID;
unordered_map <OSMID, OSMID> primaryOSMID;
unordered_map <OSMID, OSMID> secondaryOSMID;
unordered_map <OSMID, OSMID> tertiaryOSMID;
unordered_map <OSMID, OSMID> residentialOSMID;
unordered_map <OSMID, OSMID> unclassifiedOSMID;
unordered_map <OSMID, OSMID> roadsOSMID;

unordered_map <OSMID, LatLon> OSMID_LatLon_unordered_map;
unordered_map <unsigned, vector<OSMID>> wayIndex_OSMIDSubwayVector_unordered_map;

vector<Node> Nodes; //vector containing Node objects where the vector index corresponds to the intersectionID which is also the nodeID of the object
unordered_map<string, vector<unsigned>> POIName_Index_Map; //when entered

/*============================================================================================================================*/
vector<unsigned> closestIntersectionsToPOIs;
unordered_map<string, unordered_set<unsigned>> POI_closest_intersection_Map;
/*============================================================================================================================*/

//Feature data structures
vector<unsigned> UnknownFeatures;
vector<unsigned> ParkFeatures;
vector<unsigned> BeacheFeatures;
vector<unsigned> LakeFeatures;
vector<unsigned> RiverFeatures;
vector<unsigned> IslandFeatures;
vector<unsigned> ShorelineFeatures;
vector<unsigned> BuildingFeatures;
vector<unsigned> GreenspaceFeatures;
vector<unsigned> GolfcourseFeatures;
vector<unsigned> StreamFeatures;

vector<featureData> PolyFeaturesByArea;

bool initialLoad;

double maxSpeed;


//3-D R-tree vector with layers of intersections, features, POI 
//The R-tree has(x,y,LOD) as it's element




//R-tree
bgi::rtree< value, bgi::quadratic < 16, 1 >> IntersectionRtree;
bgi::rtree< value, bgi::quadratic < 16, 1 >> InterestRtree;
bgi::rtree< value, bgi::quadratic < 16, 1 >> FeatureRtree;

bgi::rtree< index_value, bgi::quadratic < 16, 1 >> IntersectionRtree_LOD;
bgi::rtree< index_value, bgi::quadratic < 16, 1 >> InterestRtree_LOD;

vector<value> intersection_in_xy;
vector<value> interest_in_xy;
vector<value> feature_in_xy;

bool load_map(std::string map_path/*map_path*/) {

    //Placeholder. This is the max speed for the world
    maxSpeed = 140;




    bool load_success = loadStreetsDatabaseBIN(map_path);
    if (!load_success) return load_success;

    string tempPath = map_path;

    tempPath.erase(tempPath.size() - 12, 12);

    string dbPathTail = ".osm.bin";

    tempPath = tempPath + dbPathTail;

    //cout << temp << endl;

    load_success = loadOSMDatabaseBIN(tempPath);
    if (!load_success) return load_success;

    //Giving Global Variables Value
    numOfStreetSegments = getNumberOfStreetSegments();
    numOfStreets = getNumberOfStreets();
    numOfIntersections = getNumberOfIntersections();
    numOfPOIs = getNumberOfPointsOfInterest();
    numOfFeatures = getNumberOfFeatures();

    street_segment_LOD.resize(numOfStreetSegments);





    for (unsigned i = 0; i < getNumberOfNodes(); i++) {
        string tag, tagValue, key, data;
        const OSMNode* node = getNodeByIndex(i);
        OSMID OSMIDsubway = node->id();
        LatLon nodeCoords = node->coords();
        OSMID_LatLon_unordered_map.insert(make_pair(OSMIDsubway, nodeCoords));

        //        for (unsigned j = 0; j < getTagCount (node); j++) {
        //            tie (tag, tagValue) = getTagPair (node, j);
        //            if (tag == "place=") {
        //                if (tagValue == "city") {
        //                    if (tagValue == "name") {
        //                        node.
        //                    }
        //                }
        //                cout <<"something"<<endl;
        //            }
        //        }

    }



    for (unsigned p = 0; p < getNumberOfWays(); p++) {
        OSMID nodeOSM = getWayByIndex(p)->id();
        for (unsigned q = 0; q < getTagCount(getWayByIndex(p)); q++) {
            string key, data;
            tie(key, data) = getTagPair(getWayByIndex(p), q);

            if (key == "highway") {
                if (data == "motorway") {
                    motorwayOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "motorway_link") {
                    motorwayLinkOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "trunk") {
                    trunkOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "trunk_link") {
                    trunkLinkOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "primary" || data == "primary_link") {
                    primaryOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "secondary" || data == "secondary_link") {
                    secondaryOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "tertiary" || data == "tertiary_link") {
                    tertiaryOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "residential" || data == "living_street") {
                    residentialOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else if (data == "unclassified") {
                    unclassifiedOSMID.insert(make_pair(nodeOSM, nodeOSM));
                } else {
                    roadsOSMID.insert(make_pair(nodeOSM, nodeOSM));
                }
            } else if (key == "railway" && data == "subway") {
                wayIndex_OSMIDSubwayVector_unordered_map.insert(make_pair(p, getWayByIndex(p)->ndrefs()));
            }
        }

    }


    /*HASHTABLE FOR INTERSECTIONS FOR STREET NAME*/

    //     std::vector<unsigned> streetsWithThisName; //Creating the vector we will return
    //
    //    //Cycling through the streets by streetID (0 to numOfStreets -1)
    //    
    //    
    //    for (unsigned counter = 0; counter < numOfStreets; counter++) {
    //        
    //        if (street_names_by_index[counter] == street_name) {
    //            streetsWithThisName.push_back(counter); //Adding ID into the vector
    //            //cout << "counter";
    //        }
    //    }


    /*------------------------------------------------------------------*/

    /*MAKE DATA STRUCTURE TO STORE STREET SGEMENTS FOR EACH INTERSECTION.
     WILL MAKE INTERSECTION AND STREET FUNCTIONS FASTER. PROBABLY BEST TO USE VECTOR INSIDE ARRAY*/
    for (unsigned i = 0; i < numOfIntersections; i++) {

        // get_intersection_position.push_back(getIntersectionPosition(i));
        street_segments_for_intersection_id.push_back(vector<unsigned>());
        street_seg_count.push_back(getIntersectionStreetSegmentCount(i));
        //street_seg_count[i].push_back(getIntersectionStreetSegmentCount(i));
        for (unsigned j = 0; j < getIntersectionStreetSegmentCount(i); j++) {

            street_segments_for_intersection_id[i].push_back(getIntersectionStreetSegment(i, j));
        }
    }

    //Initializing vectors of streets
    for (unsigned sCounter = 0; sCounter < numOfStreets; sCounter++) {
        street_street_segments.push_back(std::vector<unsigned>());
        street_all_intersections.push_back(std::vector<unsigned>());
        street_names_by_index.push_back(getStreetName(sCounter));
    }


    //   vector<unsigned> all_intersections;
    //    for(unsigned i = 0; i < numOfStreets; i++) {
    //        //intersections_for_street_name[street_names_by_index[i]];
    //        for (unsigned j = 0; j < numOfStreets; j++) {
    //            if (street_names_by_index[i] == street_names_by_index[j]) {
    //                all_intersections.push_back(j);
    //            }
    //            
    //        }
    //        intersections_for_street_name[street_names_by_index[i]] = all_intersections;
    //    }


    /*STRUCTURE FOR getStreetSegmentInfo function*/
    for (unsigned i = 0; i < numOfStreetSegments; i++) {

        //Filling streetSegInfo Vector
        street_seg_info.push_back(getStreetSegmentInfo(i));

        //Filling street street segments vector
        street_street_segments[street_seg_info[i].streetID].push_back(i);

        //Initializing the segment curve point vector
        street_seg_curve_point.push_back(vector<LatLon>());

        for (unsigned k = 0; k < street_seg_info[i].curvePointCount; k++) {
            street_seg_curve_point[i].push_back(getStreetSegmentCurvePoint(i, k));
        }

    }


    /*------------------------------------------------------------------------------------------*/


    /*------------------------------------------------------------------------------------------*/

    /*STRUCTURE FOR POI-ID LATLON*/

    for (unsigned i = 0; i < numOfPOIs; i++) {
        LatLon POIposition = getPointOfInterestPosition(i);
        POI_LatLon.push_back(POIposition);

        vector<double> tempXY = LatLon_to_XY(POI_LatLon[i].lat(), POI_LatLon[i].lon());

        POIdata tempData;

        tempData.coodinates.x = tempXY[0];
        tempData.coodinates.y = tempXY[1];
        tempData.POIName = getPointOfInterestName(i);

        POI_Data.push_back(tempData);
        hash_POIID.insert(make_pair(getPointOfInterestName(i), i));


    }
    //    unordered_set<string>POINameSet;
    //   //POI_closest_intersection_Map
    //    for (unsigned i = 0; i < POI_Data.size(); i++) { //getting POI indices for different POI names and storing in unordered_map
    //        string POIName = POI_Data[i].POIName;        //getting POIName
    //        vector<unsigned>indexPOI;   //vector in which the indices will be stored
    //        
    //        auto iter = POINameSet.find(POIName);   //find POIName in name set to see if the indices for this POI has already been entered
    //        if (iter == POINameSet.end()) {         //if not
    //            POINameSet.insert(POIName);         //insert it into name list 
    //            for (unsigned j = 0; j < POI_Data.size(); j++) {    //iterate through all POIData and find the ones with same name and insert index into vector
    //                if (POI_Data[j].POIName == POIName) {
    //                    indexPOI.push_back(j);
    //                }
    //            }
    //            POIName_Index_Map[POIName] = indexPOI;          //ex: POIName_Index_Map["Tim Horton's"] = vector{123, 45 6, 765,7234 ,234};
    //        }
    //    }
    //    
    //    
    //    //using this for loop to convert all POI locations to closest intersections and store it according to POI name in unordered map
    //    int counter = 0;
    //    for (auto iter = POIName_Index_Map.begin(); iter != POIName_Index_Map.end(); iter++) {
    //        unordered_set<unsigned>closest_intersections;
    //        vector<unsigned> POI_Indices = iter->second;  //getting the vector of POI indices for that name entry in the unordered_map
    //        for (unsigned i = 0; i < POI_Indices.size();i++) {  //going through all entries in vector and finding closest intersection to insert into closes_intersections unordered_set
    //            counter++;
    //            if (counter == 8495) {
    //                cout<<"reached this point"<<endl;
    //            }
    //            cout<<"counter: " <<counter<<endl;
    //            cout<<"lat: " <<POI_LatLon[POI_Indices[i]].lat()<<endl; //debugging message
    //            closest_intersections.insert(find_closest_intersection(POI_LatLon[POI_Indices[i]])); //inserting into unordered_set
    //        }
    //        
    //        POI_closest_intersection_Map[iter->first] = closest_intersections; //assigning set to name
    //        
    //    }


    //    /*------------------------------------------------------------------------------------intersection_nodes-*/
    //intersection_nodes.resize(numOfIntersections); //initialize space in vector for nodes as the same number of intersections




    for (unsigned i = 0; i < numOfIntersections; i++) {
        vector <unsigned> all_adjacent_intersections = find_adjacent_intersections(i); //getting adjacent intersections to that node/intersection
        Node node = Node(i, street_segments_for_intersection_id[i], all_adjacent_intersections); //creating Node objects by calling constructor in and insert inside the vector
        Nodes.push_back(node); //insert node inside vector
        Intersection_LatLon.push_back(getIntersectionPosition(i));
        vector<double> temp = LatLon_to_XY(Intersection_LatLon[i].lat(), Intersection_LatLon[i].lon());

        interXY tempPos;

        tempPos.x = temp[0];
        tempPos.y = temp[1];

        intersection_XY.push_back(tempPos);
    }



    //    /*------------------------------------------------------------------------------------------*/


    for (unsigned streetC = 0; streetC < numOfStreets; streetC++) {

        vector<unsigned> streetSegs = street_street_segments[streetC];

        hash_streetID.insert(make_pair(street_names_by_index[streetC], streetC));
        //sss => street street segments
        for (unsigned sssCount = 0; sssCount < streetSegs.size(); sssCount++) {
            unsigned intTo, intFrom;
            intTo = street_seg_info[streetSegs[sssCount]].to;
            intFrom = street_seg_info[streetSegs[sssCount]].from;

            street_all_intersections[streetC].push_back(intTo);
            street_all_intersections[streetC].push_back(intFrom);

        }
        sort(street_all_intersections[streetC].begin(), street_all_intersections[streetC].end());
        street_all_intersections[streetC].erase(std::unique(street_all_intersections[streetC].begin(), street_all_intersections[streetC].end()), street_all_intersections[streetC].end());

    }

    for (unsigned segC = 0; segC < numOfStreetSegments; segC++) {
        float distance = 0;
        if (street_seg_info[segC].curvePointCount == 0) {
            distance = find_distance_between_two_points(Intersection_LatLon[street_seg_info[segC].from], Intersection_LatLon[street_seg_info[segC].to]);
        } else {



            distance = distance + find_distance_between_two_points(
                    Intersection_LatLon[street_seg_info[segC].from],
                    street_seg_curve_point[segC][0]
                    ); //Adding the distance from from to 1st curve point

            distance = distance + find_distance_between_two_points(
                    street_seg_curve_point[segC][street_seg_info[segC].curvePointCount - 1],
                    Intersection_LatLon[street_seg_info[segC].to]
                    ); //Adding the distance from the last curve point to to

            for (unsigned i = 0; i < street_seg_info[segC].curvePointCount - 1; i++) {
                distance = distance + find_distance_between_two_points(
                        street_seg_curve_point[segC][i],
                        street_seg_curve_point[segC][i + 1]
                        ); //Distance between all the curves points. From point and to point are not included inside the curve points
            }



        }
        street_seg_length.push_back(distance);

        street_seg_travel_time.push_back(street_seg_length[segC] / (street_seg_info[segC].speedLimit / 3.6));
    }


    for (unsigned i = 0; i < numOfFeatures; i++) {

        featureData tempData;

        tempData.name = getFeatureName(i);
        tempData.type = getFeatureType(i);
        tempData.index = i;

        unsigned tempNumOfPoints = getFeaturePointCount(i);

        for (unsigned c = 0; c < tempNumOfPoints; c++) {

            LatLon tempLatLon = getFeaturePoint(i, c);
            t_point tempPos;

            vector<double> temp = LatLon_to_XY(tempLatLon.lat(), tempLatLon.lon());

            tempPos.x = temp[0];
            tempPos.y = temp[1];

            tempData.boundaryPoints.push_back(tempPos);

        }

        feature_data.push_back(tempData);


        if (tempData.type == 0) UnknownFeatures.push_back(i);
        else if (tempData.type == 1) ParkFeatures.push_back(i);
        else if (tempData.type == 2) BeacheFeatures.push_back(i);
        else if (tempData.type == 3 && tempData.name != "<noname>") LakeFeatures.push_back(i);
        else if (tempData.type == 4) RiverFeatures.push_back(i);
        else if (tempData.type == 5) IslandFeatures.push_back(i);
        else if (tempData.type == 6) ShorelineFeatures.push_back(i);
        else if (tempData.type == 7) BuildingFeatures.push_back(i);
        else if (tempData.type == 8) GreenspaceFeatures.push_back(i);
        else if (tempData.type == 9) GolfcourseFeatures.push_back(i);
        else if (tempData.type == 10) StreamFeatures.push_back(i);


    }


    double LargestLat = -180;
    double SmallestLat = 180;
    for (unsigned i = 0; i < Intersection_LatLon.size(); i++) {
        double intersectionLat = Intersection_LatLon[i].lat();

        if (intersectionLat > LargestLat)
            LargestLat = intersectionLat;
        if (intersectionLat < SmallestLat)
            SmallestLat = intersectionLat;

    };
    for (unsigned i = 0; i < POI_LatLon.size(); i++) {
        double interestLat = POI_LatLon[i].lat();

        if (interestLat > LargestLat)
            LargestLat = interestLat;
        if (interestLat < SmallestLat)
            SmallestLat = interestLat;
    };

    LatAve = (LargestLat + SmallestLat) / 2;

    for (unsigned i = 0; i < Intersection_LatLon.size(); i++) {
        double cox = LatAve*DEG_TO_RAD;
        float x1 = Intersection_LatLon[i].lon()*(cos(cox)) * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
        float y1 = Intersection_LatLon[i].lat() * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
        point Coordinate(point(x1, y1));


        IntersectionRtree.insert(std::make_pair(Coordinate, i));
    }

    for (unsigned i = 0; i < POI_LatLon.size(); i++) {
        double cox = LatAve*DEG_TO_RAD;
        float x1 = POI_LatLon[i].lon()*(cos(cox)) * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
        float y1 = POI_LatLon[i].lat() * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
        point Coordinate(point(x1, y1));
        InterestRtree.insert(std::make_pair(Coordinate, i));
    }


    vector <unsigned> segs_connected_to_intersection; //will contain all streets seg ids connected to a particular intersection
    for (unsigned i = 0; i < numOfIntersections; i++) {

        segs_connected_to_intersection = street_segments_for_intersection_id[i];

        for (unsigned j = 0; j < segs_connected_to_intersection.size(); j++) {


            //Setting up and preparing data structure access for every street type
            unordered_map <OSMID, OSMID>::const_iterator foundMotorway = motorwayOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundMotorwayLink = motorwayLinkOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundTrunk = trunkOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundTrunkLink = trunkLinkOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundPrimary = primaryOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundSecondary = secondaryOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundTertiary = tertiaryOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundUnclassified = unclassifiedOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);

            unordered_map <OSMID, OSMID>::const_iterator foundResidential = residentialOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);


            if (foundResidential != residentialOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 7;

            } else if (foundUnclassified != unclassifiedOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 7;

            } else if (foundTertiary != tertiaryOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 7;

            } else if (foundSecondary != secondaryOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 10;

            } else if (foundPrimary != primaryOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 10;

            } else if (foundTrunkLink != trunkLinkOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 11;

            } else if (foundTrunk != trunkOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 11;

            } else if (foundMotorwayLink != motorwayLinkOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 11;

            } else if (foundMotorway != motorwayOSMID.end()) {

                street_segment_LOD[segs_connected_to_intersection[j]] = 11;


            }

        }
    }


    for (unsigned i = 0; i < numOfPOIs; i++) {
        closestIntersectionsToPOIs.push_back(find_closest_intersection(POI_LatLon[i]));
    }

    unordered_set<string> nameSet;

    for (unsigned i = 0; i < closestIntersectionsToPOIs.size(); i++) {

        //POI_closest_intersection_Map
        //POI_Data
        unordered_set<unsigned> closest_to_inter;
        string nameOfPOI = POI_Data[i].POIName;

        auto iter = nameSet.find(nameOfPOI); //check if POI of this name is already entered into the map
        if (iter == nameSet.end()) { //if not then enter it and find the closest intersections to all the POIs with that name
            for (unsigned j = 0; j < closestIntersectionsToPOIs.size(); j++) {
                if (POI_Data[j].POIName == nameOfPOI) {
                    closest_to_inter.insert(closestIntersectionsToPOIs[j]);
                }
            }
            POI_closest_intersection_Map[nameOfPOI] = closest_to_inter;
        }


    }
    
    nameSet.clear();





    //Diagnostic timer

    return load_success;


}

void close_map() {
    //Clean-up your map related data structures here
    //Invert of the Load map, to be done monday

    //R-tree
    IntersectionRtree.clear();
    InterestRtree.clear();
    FeatureRtree.clear();

    IntersectionRtree_LOD.clear();
    InterestRtree_LOD.clear();

    //Two point vectors use to populate rtree
    intersection_in_xy.clear();
    interest_in_xy.clear();
    feature_in_xy.clear();



    POI_Data.clear();
    hash_POIID.clear();
    intersection_XY.clear(); //Vector containing all intersection XY coordinates

    for (unsigned i = 0; i < numOfFeatures; i++) {
        feature_data[i].boundaryPoints.clear();
    }
    feature_data.clear(); //Vector containing all the feature bounds in XY coords

    street_segment_LOD.clear(); // LOD for each street segment by index

    motorways.clear();

    motorwayOSMID.clear();
    motorwayLinkOSMID.clear();
    trunkOSMID.clear();
    trunkLinkOSMID.clear();
    primaryOSMID.clear();
    secondaryOSMID.clear();
    tertiaryOSMID.clear();
    residentialOSMID.clear();
    unclassifiedOSMID.clear();
    roadsOSMID.clear();
    OSMID_LatLon_unordered_map.clear();
    wayIndex_OSMIDSubwayVector_unordered_map.clear();

    for (unsigned count = 0; count < numOfStreets; count++) {
        street_street_segments[count].clear();
        street_all_intersections[count].clear();
    }

    for (unsigned count = 0; count < numOfIntersections; count++) {
        street_segments_for_intersection_id[count].clear();

    }

    for (unsigned count = 0; count < numOfStreetSegments; count++) {
        street_seg_curve_point.clear();
    }

    street_seg_length.clear();

    street_seg_count.clear();
    street_seg_info.clear();
    street_names_by_index.clear();
    Intersection_LatLon.clear();
    street_seg_curve_point_num.clear();
    street_seg_travel_time.clear();
    ParkFeatures.clear();
    BeacheFeatures.clear();
    LakeFeatures.clear();
    RiverFeatures.clear();
    IslandFeatures.clear();
    ShorelineFeatures.clear();
    BuildingFeatures.clear();
    GreenspaceFeatures.clear();
    GolfcourseFeatures.clear();
    StreamFeatures.clear();
    UnknownFeatures.clear();

    POIName_Index_Map.clear(); //when entered

    closestIntersectionsToPOIs.clear();
    POI_closest_intersection_Map.clear();

    POI_LatLon.clear();

    street_street_segments.clear();

    hash_streetID.clear();
    Nodes.clear();
    closeOSMDatabase();
    closeStreetDatabase(); // Final Closing of everything


}

double find_distance_between_two_points(LatLon point1, LatLon point2) {

    double x1, y1, x2, y2, latAve;

    //1-pow(cox,2.0)/2+pow(cox,4.0)/24-pow(cox,6.0)/720+pow(cox,8.0/40320)
    latAve = (point1.lat() + point2.lat()) / 2;
    double cox = latAve*DEG_TO_RAD;
    x1 = point1.lon()*(cos(cox)) * DEG_TO_RAD;
    x2 = cos(latAve * DEG_TO_RAD) * point2.lon() * DEG_TO_RAD;
    y1 = point1.lat() * DEG_TO_RAD;
    y2 = point2.lat() * DEG_TO_RAD;

    return (EARTH_RADIUS_IN_METERS * sqrt((pow((x2 - x1), 2.0))+(pow((y2 - y1), 2.0))));

}
//;

double find_street_length(unsigned street_id) {
    std::vector<unsigned> vect = find_street_street_segments(street_id);
    double distance = 0;
    for (unsigned i = 0; i < vect.size(); i++) {

        distance = distance + find_street_segment_length(vect[i]);

    }

    return distance;


}

double find_street_segment_travel_time(unsigned street_segment_id) {
    return street_seg_travel_time[street_segment_id];
}

double find_street_segment_length(unsigned street_segment_id) {
    return street_seg_length[street_segment_id];

}

unsigned find_closest_point_of_interest(LatLon my_position) {

    //    double shortest = 10000000;
    //    double distance;
    //    unsigned index = 0;
    //    for (unsigned i = 0; i < POI_LatLon.size(); i++) {
    //        distance = find_distance_between_two_points(POI_LatLon[i], my_position);
    //
    //        if (distance < shortest) {
    //            shortest = distance;
    //            index = i;
    //        }
    //    }
    //    return index;

    double cox = LatAve*DEG_TO_RAD;
    double x1 = my_position.lon()*(cos(cox)) * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
    double y1 = my_position.lat() * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
    point Coordinate(x1, y1);

    vector<value> nearest_value;


    InterestRtree.query(bgi::nearest(Coordinate, 1), back_inserter(nearest_value));
    return nearest_value[0].second;

}

unsigned find_closest_intersection(LatLon my_position) {
    double cox = LatAve*DEG_TO_RAD;
    double x1 = my_position.lon()*(cos(cox)) * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
    double y1 = my_position.lat() * DEG_TO_RAD*EARTH_RADIUS_IN_METERS;
    point Coordinate(x1, y1);

    vector<value> nearest_value;


    IntersectionRtree.query(bgi::nearest(Coordinate, 1), back_inserter(nearest_value));
    return nearest_value[0].second;
}

//Returns the street segments for the given intersection 

vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {
    return street_segments_for_intersection_id[intersection_id];
}


//Returns the street names at the given intersection (includes duplicate street names in returned vector)

vector<string> find_intersection_street_names(unsigned intersection_id) {
    unsigned num_streets_on_intersection = street_seg_count[intersection_id];
    //find all street segments ids connected to the intersection
    //call getStreetSegmentInfo and give it the ids and obtain streetID
    //push the streetIDs in a vector
    //for every element in that vector call getStreetName and store the returned string in a vector
    //return vector

    vector <unsigned> street_segs_on_intersection;
    vector <unsigned> street_IDs;
    vector <string> street_names;
    for (unsigned i = 0; i < num_streets_on_intersection; i++) {
        street_segs_on_intersection.push_back(street_segments_for_intersection_id[intersection_id][i]);
    }

    for (unsigned i = 0; i < street_segs_on_intersection.size(); i++) {
        street_IDs.push_back((street_seg_info[street_segs_on_intersection[i]]).streetID);
    }

    for (unsigned i = 0; i < street_IDs.size(); i++) {
        street_names.push_back(getStreetName(street_IDs[i]));
    }

    return street_names;

}

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {

    unsigned num_streets_intersection1 = street_seg_count[intersection_id1]; //getting number of streets on intersection1
    unsigned num_streets_intersection2 = street_seg_count[intersection_id2]; //getting number of streets on intersection2

    // using double for loop to cross reference each street segment connecting to both
    // intersections. If there is a common street segment, the streets are connected
    // Order n^2
    for (unsigned i = 0; i < num_streets_intersection1; i++) {
        for (unsigned j = 0; j < num_streets_intersection2; j++) {
            if (street_segments_for_intersection_id[intersection_id1][i]
                    == street_segments_for_intersection_id[intersection_id2][j]) {
                return true;
            }
        }
    }

    return false;

}


//Returns all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections

vector<unsigned> find_adjacent_intersections(unsigned intersection_id) {
    vector<unsigned> intersections; //vector that will hold all the intersections
    vector<unsigned> valid_street_segs;
    vector<unsigned> street_segs_on_intersection; //holds all connecting street segments on that intersection
    StreetSegmentInfo street_segment_info;

    unsigned num_streets_on_intersection = street_seg_count[intersection_id]; //get # of streets connected to that intersection


    //iterate through all the street segments connected to the intersection
    //intersection_id_of_oneway_to contains all the intersections that these street segments lead to
    //if intersection_id_of_oneway isn't the same as the intersection ID passed into the function insert the street segment id into street_segs_on_intersection
    for (unsigned i = 0; i < num_streets_on_intersection; i++) {
        street_segs_on_intersection.push_back(getIntersectionStreetSegment(intersection_id, i));

    }


    for (unsigned i = 0; i < street_segs_on_intersection.size(); i++) {
        street_segment_info = street_seg_info[street_segs_on_intersection[i]];

        if (street_segment_info.oneWay == true && (street_segment_info.to) != intersection_id
                && (find(intersections.begin(), intersections.end(), street_segment_info.to) == intersections.end())) {

            intersections.push_back(street_segment_info.to);

        } else if (street_segment_info.oneWay == true && (street_segment_info.to) == intersection_id
                && !(find(intersections.begin(), intersections.end(), street_segment_info.to) == intersections.end())) {
            //            
        } else if (street_segment_info.to == intersection_id && street_segment_info.oneWay == false
                && (find(intersections.begin(), intersections.end(), street_segment_info.from) == intersections.end())) {

            intersections.push_back(street_segment_info.from);

        } else if (street_segment_info.from == intersection_id && street_segment_info.oneWay == false
                && (find(intersections.begin(), intersections.end(), street_segment_info.to) == intersections.end())) {

            intersections.push_back(street_segment_info.to);
        }
    }

    return intersections;
}

//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
//However street names are not guarenteed to be unique, so more than 1 intersection id may exist

vector<unsigned> find_intersection_ids_from_street_names(string street_name1, string street_name2) {
    vector<unsigned> intersection_ids;

    vector<unsigned> street1_ids = find_street_ids_from_name(street_name1); //gets me the street ids for street_name1
    vector<unsigned> street2_ids = find_street_ids_from_name(street_name2); //gets me the street ids for street_name2

    vector<unsigned> all_intersections_on_street1; // contains all intersection on the street_name1;
    vector<unsigned> all_intersections_on_street2; // contains all intersection on the street_name2;

    if (street1_ids.size() != 1) {
        //more than one street with the same name
        for (unsigned i = 0; i < street1_ids.size(); i++) {
            vector<unsigned> temp_vector = street_all_intersections[street1_ids[i]];
            for (unsigned j = 0; j < temp_vector.size(); j++) {
                all_intersections_on_street1.push_back(temp_vector[j]);
            }

        }
    } else {
        for (unsigned i = 0; i < street1_ids.size(); i++) {
            all_intersections_on_street1 = street_all_intersections[street1_ids[i]]; //putting all street intersections on street 1 in all_intersections_on_street1
        }
    }

    if (street2_ids.size() != 0) {
        //more than one street with the same name
        for (unsigned i = 0; i < street2_ids.size(); i++) {
            vector<unsigned> temp_vector = street_all_intersections[street2_ids[i]];
            for (unsigned j = 0; j < temp_vector.size(); j++) {
                all_intersections_on_street2.push_back(temp_vector[j]);
            }
        }
    } else {
        for (unsigned i = 0; i < street2_ids.size(); i++) {
            all_intersections_on_street2 = street_all_intersections[street2_ids[i]]; //putting all street intersections on street 2 in all_intersections_on_street2

        }
    }

    for (unsigned i = 0; i < all_intersections_on_street1.size(); i++) {
        if (find(all_intersections_on_street2.begin(), all_intersections_on_street2.end(), all_intersections_on_street1[i]) != all_intersections_on_street2.end()) {
            intersection_ids.push_back(all_intersections_on_street1[i]);
        }
    }
    return intersection_ids;

}


//Returns all street segments for the given street

std::vector < unsigned > find_street_street_segments(unsigned street_id) {
    return street_street_segments[street_id];
}

vector<unsigned> find_street_ids_from_name(std::string street_name) {

    auto streetIds = hash_streetID.equal_range(street_name);

    vector<unsigned> streetsWithThisName;
    for (auto iter = streetIds.first; iter != streetIds.second; iter++) {
        streetsWithThisName.push_back(iter->second);
    }

    return streetsWithThisName;

}

vector < unsigned > find_all_street_intersections(unsigned street_id) {
    return street_all_intersections[street_id];
}





