/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2.h
 * Author: zhan2860
 *
 * Created on February 14, 2017, 10:27 AM
 */

#ifndef M2MOD_H
#define M2MOD_H
using namespace std;
#include "graphics_types.h"
#include "DataStructures.h"


//assistance function, 
void delay(long milliseconds);

void draw_streets (int level_of_detail);

bool UserLoadMap();

void draw_screen();

void act_on_button_press(float x, float y, t_event_buttonPressed event);

vector<unsigned> find_intersection();

void drawVerticalRoad (t_point start_point, t_point end_point);

void drawIntersectionMarker();

void draw_subwayTracks (int zoom_level);

void act_on_find_button_func(void (*drawscreen_ptr) (void));

void act_on_help(void (*drawscreen_ptr) (void));

void drawMarker ();

void drawSearchBar ();

//Draws markers for POIs
//Toggle-able (needs to be implemented);
//Loads marker image from png?
void draw_POIs(int level_of_detail);

//Draws all the features
//Needs to be toggle-able
//Different features are differently colored.
void draw_features(int level_of_detail);

vector<double> find_map_boundary ();

vector<double> latLon_to_cartesian (double min_Lat, double max_Lat, double min_Lon, double max_Lon);

//Function to return the xy coordinates form a single LatLon point
//return vector is X then Y
vector<double> LatLon_to_XY(double Lat, double Lon);


void act_on_mouse_move(float x, float y);

void act_on_key_press(char c, int keysym);

//unused
void draw_string_along_points(vector<t_point> the_curve, float km_per_character_on_screen, string name);

t_point LatLon_to_t_point(LatLon position);

float distance_between_two_t_points(t_point point1,t_point point2);

float find_km_per_char();

void draw_between_two_point(t_point point1,t_point point2,string name,bool one_way);

void draw_street_segment_name(int LOD,unsigned seg_index);

int current_level_of_detail();

void draw_streets_name(int level_of_detail);

LatLon XY_to_LatLon(float x, float y);

void place_marker(float x, float y, int LOD); //Draws a marker at the clicked location
void place_Dirmarker(float x, float y, int LOD);
void place_start_marker();


void draw_intersection_text_box(float x, float y, int LOD);// Draws a floating text box above input location

void draw_initialLoadingScreen();// Inital demand for user map path and OSMID data input

void delay(int frames);

void draw_street_seg(unsigned seg_ID, int level_of_detail);


//Test box for Hover POIs
void draw_POI_text_box(float x, float y, int LOD);

void draw_path(std::vector<unsigned> Segs);

void draw_direction_list(std::vector<unsigned> SegsID);

//turn right ==1,turn left ==2, straight==3, U turn ==4
//moving from segIDfrom to segIDto
int turn_direction(unsigned segIDfrom,unsigned segIDto);

//deltaX = point2x-point1x
//deltaY = point2y-point1y
float find_angle_between_point_in_degree(float deltaX,float deltaY);

void place_end_marker ();


void act_on_path_func(void (*drawscreen_ptr) (void));
#endif
