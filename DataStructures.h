/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DataStructures.h
 * Author: rahma314
 *
 * Created on March 14, 2017, 9:09 PM
 */
using namespace std;
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include "list"
#include "math.h"
#include "Node.h"
#include "LatLon.h"
#include "OSMID.h"
#include "StreetsDatabaseAPI.h"
#include "graphics.h"
#include "OSMDatabaseAPI.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <utility>
#include <map>
#include <unordered_map>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/foreach.hpp>
#include <chrono>
#include <thread>




struct POIdata{
    t_point coodinates;
    string POIType;
    string POIName;
};

struct interXY{
    double x;
    double y;
    
};

struct featureData{
    vector<t_point> boundaryPoints;
    unsigned type;
    string name;
    unsigned index;
};

/* ----------------- ALL GLOBAL VARIABLES -----------------*/
struct intersection_data {
    LatLon position;
    string name;
};


////////////////////////////////////////m2Mod.h////////////////////////////////////
//Minor Structures for data



#endif /* DATASTRUCTURES_H */

