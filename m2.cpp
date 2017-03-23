/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2.cpp
 * Author: zhan2860
 * 
 * Created on February 14, 2017, 10:27 AM
 */


#include "m2.h"
#include "m3.h"
#include "m1.h"
#include "m2Mod.h"
#include "m3Mod.h"
#include "Global_Variables.h"




using namespace std;
//
//vector<double> latLon_to_cart (double min_Lat, double max_Lat, double min_Lon, double max_Lon);
//void draw_screen();
t_point rubber_pt; // Last point to which we rubber-banded.
std::vector<t_point> line_pts; // Stores the points entered by user clicks.


float recentClickX, recentClickY;
int pathButtonCount = 0;
bool showOptions = false;
bool userInputing = false;
//bool directionInput = false; //if we are currently inputing

bool findTextOff = false;

bool SubwaysOn = false;
bool POIsOn = true;
bool GreenOn = true;
bool WaterOn = true;
bool BuildingOn = true;

bool findButtonClicked = false;
bool findHelpClicked = false;
vector<unsigned> path;
bool drawIntersectionData = false;
bool intersectionClicked = false;
bool pathButtonPressed = false;
bool POIHovered = false;
bool pathFound = false;
double XStretchFactor = 2;
double YStretchFactor = 2.35;
double currentMouseX, currentMouseY;
unsigned findPathSrc;
unsigned findPathDest;
vector <double> xyCoordsOfIntersection;
unsigned nearest_int;
float km_per_char;
//vector <intersection_data> intersections; // vector containing intersection_data { LatLon position; string name;}

t_color orangeColor = t_color(231, 130, 0, 130);
t_color yellowColor = t_color(237, 214, 123, 230);

string mapName;
string mapPath;
string dbPath;
vector <unsigned> pathButtonPath;
vector<unsigned> actualPath;
double average_lat; //average latitude for the map
vector <double> coords; //Coordinates for visible world

string keyboardBuff; //Temp storage for input.

/*------------------------------------------------------------*/

int markCount = 0;

bool UserLoadMap() {


    string mapPathHead = "/cad2/ece297s/public/maps/";
    string mapPathTail = ".streets.bin";

    cout << "Enter an new map name (\"city_country\" or just \"country\" for small states) ";
    cout << endl << "Type \"exit\" to exit " << endl;
    cin >> mapName;

    if (mapName == "exit") {
        endProgram = true;

        return true;
    }


    mapPath = mapPathHead + mapName + mapPathTail;


    bool success = load_map(mapPath);

    if (!success) {
        cout << endl << "Invalid Map or Database Path" << endl;
        return false;
    }

    initialLoad = true;

    return success;


}

void draw_screen() {


    //    coords = find_map_boundary();
    //    if(!LOD_area_test((coords[2]-coords[0])*(coords[3]-coords[1]))){    
    //        set_visible_world(coords[0], coords[1], coords [2], coords[3]);
    //    }

    int zoom_level = current_level_of_detail(); //Pass this to ALL Draw Functions


    clearscreen();



    draw_features(zoom_level);
    draw_streets(zoom_level);
    draw_subwayTracks(zoom_level);


    for (unsigned i = 0; i < path.size(); i++) {
        draw_street_seg(path[i], 0);
    }
    //draw_street_seg(20772, 0);
    //    
    for (unsigned i = 0; i < actualPath.size(); i++) {
        draw_street_seg(actualPath[i], 0);
    }


    //drawtext(get_visible_world().get_xcenter(),get_visible_world().get_ycenter(),"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
    //    draw_streets_name(zoom_level);

    for (unsigned c = 0; c < numOfStreetSegments; c++) {
        draw_street_segment_name(zoom_level, c);
    }



    draw_POIs(zoom_level);


    //    //drawSearchBar();
    //    if (findButtonClicked) {
    //        drawMarker();
    //    }

    if (intersectionClicked) {
        if (!pathButtonPressed) findPathSrc = nearest_int;
        place_marker(recentClickX, recentClickY, zoom_level);
        draw_intersection_text_box(recentClickX, recentClickY, zoom_level);

    }

    if (pathFound) {
        for (unsigned i = 0; i < pathButtonPath.size(); i++) {
            draw_street_seg(pathButtonPath[i], 0);
        }
        pathButtonPressed = false;
    }

    if (findButtonClicked) {
        drawMarker();
    }

    if (POIHovered && zoom_level < 5) {
        double tempx;
        double tempy;

        tempx = currentMouseX;
        tempy = currentMouseY;

        draw_POI_text_box(tempx, tempy, zoom_level);
    }

    if (findHelpClicked) {


        set_coordinate_system(GL_SCREEN);

        Surface InterHelp = load_png_from_file("/nfs/ug/homes-5/l/linnaihs/Documents/mapper/PNGs/Intersectionhelp.png");
        Surface ProcHelp = load_png_from_file("/nfs/ug/homes-5/l/linnaihs/Documents/mapper/PNGs/Proceedhelpbutton.png");
        Surface ToggleHelp = load_png_from_file("/nfs/ug/homes-5/l/linnaihs/Documents/mapper/PNGs/Togglebuttons.png");
        Surface ZoomHelp = load_png_from_file("/nfs/ug/homes-5/l/linnaihs/Documents/mapper/PNGs/Zoomhelp.png");
        Surface WindowHelp = load_png_from_file("/nfs/ug/homes-5/l/linnaihs/Documents/mapper/PNGs/windowhelp.png");
        Surface Help = load_png_from_file("/nfs/ug/homes-5/l/linnaihs/Documents/mapper/PNGs/Help.png");

        draw_surface(InterHelp, t_point(500, 300));
        draw_surface(ProcHelp, t_point(1150, 320));
        draw_surface(ToggleHelp, t_point(700, 690));
        draw_surface(ZoomHelp, t_point(1140, 77));
        draw_surface(WindowHelp, t_point(1075, 185));
        draw_surface(Help, t_point(857, 237));

        set_coordinate_system(GL_WORLD);

    }
    //draw_direction_list(path);
    copy_off_screen_buffer_to_screen();

    //cout << "Screen Drawn" << endl;
    //find_intersection();
}

void draw_map() {


    init_graphics(mapName, t_color(224, 224, 224));


    coords = find_map_boundary();

    set_visible_world(coords[0], coords[1], coords [2], coords[3]);
    //cout << setprecision(12) <<coords[0] <<" " << coords[1] << " " << coords[2] << " " << coords[3] <<endl;


    //    intersections.resize(getNumberOfIntersections ());

    //    for (unsigned i = 0; i < intersections.size(); i++) {
    //        intersections[i].position = getIntersectionPosition(i);
    //        intersections[i].name = getIntersectionName(i);
    //    }



    set_keypress_input(true);
    set_mouse_move_input(true);
    set_drawing_buffer(OFF_SCREEN);
    create_button("Window", "Find", act_on_find_button_func); // name is UTF-8
    create_button("Find", "Help", act_on_help);
    create_button("Help", "Path", act_on_path_func);
    event_loop(act_on_button_press, act_on_mouse_move, act_on_key_press, draw_screen);

    close_graphics();
}

//called when Find button is used
//function for drawing the marker at the x-y value of the intersection

void drawMarker() {
    const char* marker_path;
    marker_path = "/nfs/ug/homes-1/z/zhan2860/Documents/ECE297/Milestone1/mapper/libstreetmap/src/easygl/small_image.png";
    Surface POImarker = load_png_from_file(marker_path);
    cout << xyCoordsOfIntersection[0] << " y:" << xyCoordsOfIntersection[1] << endl;
    draw_surface(marker_path, t_point(xyCoordsOfIntersection[0], xyCoordsOfIntersection[1]));
}

//

//given two point, draw the name on the segment and if it is one way, add an arrow on it

void act_on_path_func(void (*drawscreen_ptr) (void)) {
    pathButtonPressed = true;

    if (pathButtonPressed) {
        findPathDest = nearest_int;

        pathButtonPath = find_path_between_intersections(findPathSrc, findPathDest, 0);

        if (path.size() != 0) pathFound = true;
    }
    draw_screen();
}

void draw_between_two_point(t_point point1, t_point point2, string name, bool one_way) {
    t_point centreOfText;
    setcolor(0, 0, 0);
    float rotation;
    //        float width, height,
    //        width = 1;
    //        height = 1;
    string empty = ("<unknown>");

    double deltaX, deltaY;
    centreOfText.x = (point1.x + point2.x) / 2;
    centreOfText.y = (point1.y + point2.y) / 2;
    deltaX = point2.x - point1.x;
    deltaY = point2.y - point1.y;
    rotation = find_angle_between_point_in_degree(deltaX, deltaY);
    settextrotation(rotation);
    if (name.compare(empty) != 0) {
        if (one_way) {
            if (abs(rotation) < 90)
                name = name + "   -->";
            else {
                rotation += 180;
                name = "<--   " + name;
                settextrotation(rotation);
            }

        } else {
            if (abs(rotation) > 90) {
                rotation += 180;
                settextrotation(rotation);
            }
        }
        drawtext(centreOfText.x, centreOfText.y, name);
    }


}

//give an street segment, draw names on it

void draw_street_segment_name(int LOD, unsigned seg_index) {
    set_coordinate_system(GL_WORLD);
    if (street_segment_LOD[seg_index] > LOD) {
        unsigned string_length = street_names_by_index[street_seg_info[seg_index].streetID].length() + 2;
        float text_length_on_map = string_length*km_per_char;
        if (text_length_on_map * 1000 < find_street_segment_length(seg_index)) {



            t_point centreOfText;
            //        float rotation;
            //        float width, height,
            //        width = 1;
            //        height = 1;
            int numberOfSeg = street_seg_info[seg_index].curvePointCount;
            string empty = ("<unknown>");
            //        double deltaX, deltaY;
            if (numberOfSeg == 0) {
                draw_between_two_point(LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_index].from]), LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_index].to]), street_names_by_index[street_seg_info[seg_index].streetID], street_seg_info[seg_index].oneWay);
            } else if (numberOfSeg == 1) {

                if (text_length_on_map * 1000 < find_distance_between_two_points(Intersection_LatLon[street_seg_info[seg_index].from], street_seg_curve_point[seg_index][0])) {
                    draw_between_two_point(LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_index].from]), LatLon_to_t_point(street_seg_curve_point[seg_index][0]), street_names_by_index[street_seg_info[seg_index].streetID], street_seg_info[seg_index].oneWay);
                }
                if (text_length_on_map * 1000 < find_distance_between_two_points(Intersection_LatLon[street_seg_info[seg_index].to], street_seg_curve_point[seg_index][0])) {
                    draw_between_two_point(LatLon_to_t_point(street_seg_curve_point[seg_index][0]), LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_index].to]), street_names_by_index[street_seg_info[seg_index].streetID], street_seg_info[seg_index].oneWay);
                }
            } else {
                if (text_length_on_map * 1000 < find_distance_between_two_points(Intersection_LatLon[street_seg_info[seg_index].from], street_seg_curve_point[seg_index][0])) {
                    draw_between_two_point(LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_index].from]), LatLon_to_t_point(street_seg_curve_point[seg_index][0]), street_names_by_index[street_seg_info[seg_index].streetID], street_seg_info[seg_index].oneWay);
                }
                for (int i = 0; i < numberOfSeg - 1; i++) {

                    if (text_length_on_map * 1000 < find_distance_between_two_points(street_seg_curve_point[seg_index][i], street_seg_curve_point[seg_index][i + 1])) {
                        draw_between_two_point(LatLon_to_t_point(street_seg_curve_point[seg_index][i]), LatLon_to_t_point(street_seg_curve_point[seg_index][i + 1]), street_names_by_index[street_seg_info[seg_index].streetID], street_seg_info[seg_index].oneWay);
                    }
                    if (text_length_on_map * 1000 < find_distance_between_two_points(Intersection_LatLon[street_seg_info[seg_index].to], street_seg_curve_point[seg_index][numberOfSeg - 1])) {
                        draw_between_two_point(LatLon_to_t_point(street_seg_curve_point[seg_index][numberOfSeg - 1]), LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_index].to]), street_names_by_index[street_seg_info[seg_index].streetID], street_seg_info[seg_index].oneWay);
                    }

                }
            }
        }
    }
}

//called when Find button is pressed
//calls find_intersection to get the intersection LatLon, converts to x-y
//stores x-y value in global variable and calls draw_screen()

void act_on_find_button_func(void (*drawscreen_ptr) (void)) {

    if (findButtonClicked)findButtonClicked = false;
    else findButtonClicked = true;

    if (findButtonClicked) {
        vector<unsigned> intersectionID = find_intersection();
        cout << intersectionID.size() << endl;
        for (unsigned i = 0; i < intersectionID.size(); i++) {
            LatLon intersectionPosition = Intersection_LatLon[intersectionID[i]];
            xyCoordsOfIntersection = LatLon_to_XY(intersectionPosition.lat(), intersectionPosition.lon());
            set_visible_world(xyCoordsOfIntersection[0] + 2, xyCoordsOfIntersection[1] - 2, xyCoordsOfIntersection[0] - 2, xyCoordsOfIntersection[1] + 2);
            draw_screen();
        }

    } else {
        drawscreen_ptr();

    }



}

void act_on_help(void (*drawscreen_ptr) (void)) {
    if (findHelpClicked) {
        findHelpClicked = false;
        drawscreen_ptr();
    } else {
        findHelpClicked = true;
        drawscreen_ptr();
    }

}

void act_on_button_press(float x, float y, t_event_buttonPressed event) {

    /* Called whenever event_loop gets a button press in the graphics *
     * area.  Allows the user to do whatever he/she wants with button *
     * clicks.                                                        */

    double scale = pow(1.25, current_level_of_detail());

    LatLon mouse_pos = XY_to_LatLon(x, y);

    //    std::cout << "User clicked a mouse button at coordinates ("
    //            << mouse_pos.lon() << "," << mouse_pos.lat()<< ")";


    nearest_int = find_closest_intersection(mouse_pos);


    //    cout << "Closest Intersection: "<< nearest_int  <<endl;
    double d_between = find_distance_between_two_points(mouse_pos, Intersection_LatLon[nearest_int]);

    set_coordinate_system(GL_SCREEN);
    if ((x > 50 && x < 350 && y > 10 && y < 50) && event.button == 1) {
        findTextOff = true;
    }
    set_coordinate_system(GL_WORLD);
    //Click was within 3 meter radius of the point 
    if (d_between < 5 * scale) {


        recentClickX = x;
        recentClickY = y;

        intersectionClicked = true;
        //        if (intersectionClicked) pathButtonCount++;
        //update_message("Nice. You found and intersection."); // To be replaced with a message box with enclosed details
        draw_screen();

    } else {
        update_message("You missed.");
        intersectionClicked = false;
        pathButtonPressed = false;
        draw_screen();
    }


}

void act_on_mouse_move(float x, float y) {
    // function to handle mouse move event, the current mouse position in the current world coordinate
    // system (as defined in your call to init_world) is returned

    //std::cout << "Mouse move at " << x << "," << y << ")\n";
    //Click was within 3 meter radius of the point 
    double scale = pow(1.25, current_level_of_detail());

    LatLon mouse_pos = XY_to_LatLon(x, y);

    //    std::cout << "User clicked a mouse button at coordinates ("
    //            << mouse_pos.lon() << "," << mouse_pos.lat()<< ")";


    unsigned nearest_int = find_closest_point_of_interest(mouse_pos);


    //    cout << "Closest Intersection: "<< nearest_int  <<endl;
    double d_between = find_distance_between_two_points(mouse_pos, POI_LatLon[nearest_int]);


    //Click was within 3 meter radius of the point 
    if (d_between < 5 * scale && current_level_of_detail() < 5) {

        currentMouseX = x;
        currentMouseY = y;

        POIHovered = true;
        update_message("There is something... interesting here."); // To be replaced with a message box with enclosed details
        draw_screen();

    } else if (current_level_of_detail() < 6) {
        update_message("Nothing's here...");
        POIHovered = false;
    }
}


#include <X11/keysym.h>

//keystroke function, currently unused

void act_on_key_press(char c, int keysym) {
    // function to handle keyboard press event, the ASCII character is returned
    // along with an extended code (keysym) on X11 to represent non-ASCII
    // characters like the arrow keys.
    //Keys are used to toggle view options


    std::cout << "keysym (extended code) is " << keysym << std::endl;

#ifdef X11 // Extended keyboard codes only supported for X11 for now




    if (showOptions == true) {
        switch (keysym) {
            case XK_Left:
                //std::cout << "Left Arrow" << std::endl;
                break;
            case XK_Right:
                //std::cout << "Right Arrow" << std::endl;
                break;
            case XK_Up:
                //std::cout << "Up Arrow" << std::endl;
                initialLoad = false;
                break;
            case XK_Down:
                //std::cout << "Down Arrow" << std::endl;
                break;

            case 49:
                if (POIsOn == true) {
                    POIsOn = false;
                    update_message("POIs OFF");
                } else {
                    POIsOn = true;
                    update_message("POIs ON");
                }

                draw_screen();
                break;

            case 53:
                if (BuildingOn == true) {
                    BuildingOn = false;
                    update_message("Buildings OFF");
                } else {
                    BuildingOn = true;
                    update_message("Buildings ON");
                }

                draw_screen();
                break;

            case 50:
                if (SubwaysOn == true) {
                    SubwaysOn = false;
                    update_message("Subways OFF");
                } else {
                    SubwaysOn = true;
                    update_message("Subways ON");
                }
                draw_screen();
                break;

            case 51:
                if (WaterOn == true) {
                    WaterOn = false;
                    update_message("Water OFF");
                } else {
                    WaterOn = true;
                    update_message("Water ON");
                }
                draw_screen();
                break;

            case 52:
                if (GreenOn == true) {
                    GreenOn = false;
                    update_message("Greenspaces OFF");
                } else {
                    GreenOn = true;
                    update_message("Greenspaces ON");
                }
                draw_screen();
                break;

            case 32:
                set_visible_world(coords[0], coords[1], coords [2], coords[3]);
                draw_screen();
                break;


            default:
                update_message(" Press control (but really anything but numbers and spacebar) for options. ");
                update_message("Key press: char is " + c);
                update_message("Toggle map elements (Key) : \n (1) POIs \n (2) Subways \n (3) Water \n (4) Greenspaces \n (5) Buildings \n (SpaceBar) Reset Zoom ");
                break;
        }
    }

    if (keysym == XK_Escape) {
        if (showOptions == true) {
            showOptions = false;
            update_message("Closed Options menu");
        } else {
            showOptions = true;
            update_message("Toggle map elements (Key) : \n (1) POIs \n (2) Subways \n (3) Water \n (4) Greenspaces \n (5) Buildings \n (SpaceBar) Reset Zoom ");
        }
    }

    if (keysym == XK_Tab) {

        if (userInputing == true) {
            userInputing = false;

        } else {
            userInputing = true;
            update_message("Inputing");
        }

    }

    if (userInputing == true) {

        if (keysym == XK_BackSpace && keyboardBuff.size() > 0) {

            keyboardBuff = keyboardBuff.substr(0, keyboardBuff.size() - 1);
            update_message(keyboardBuff);

        } else {
            keyboardBuff = keyboardBuff + c;

            update_message(keyboardBuff);
        }

    }

#endif
}
//draw all the streets
//Function which draws all streets depending on LOD

//used to draw streets on the map
//checks for different kinds of streets and uses level_of_detail to filter
//out smaller streets when zoomed out and draws details as user zooms in

void draw_streets(int level_of_detail) {
    //for every intersection, get the street segments connected to it
    //get the curvePointCount for the intersections
    //if curvePointCount == 0 draw a line to, from
    //else draw lines from, from to curvePoint to, to



    int scaleFactor = pow(2, 16 / (level_of_detail + 1));




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

            unordered_map <OSMID, OSMID>::const_iterator foundRoads = roadsOSMID.find(street_seg_info[segs_connected_to_intersection[j]].wayOSMID);



            if (street_seg_info [segs_connected_to_intersection[j]].curvePointCount == 0) { //if no curve points on that street segment

                t_point start_point = t_point(intersection_XY[i].x, intersection_XY[i].y);
                t_point end_point = t_point(intersection_XY[street_seg_info[segs_connected_to_intersection[j]].to].x, intersection_XY[street_seg_info[segs_connected_to_intersection[j]].to].y);


                //checks what type of street it is and what level_of_detail the user is at
                //draws streets differently according to those conditions

                if (foundResidential != residentialOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundUnclassified != unclassifiedOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundTertiary != tertiaryOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundSecondary != secondaryOSMID.end() && level_of_detail < 10) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(yellowColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundPrimary != primaryOSMID.end() && level_of_detail < 10) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(yellowColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundTrunkLink != trunkLinkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundTrunk != trunkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundMotorwayLink != motorwayLinkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundMotorway != motorwayOSMID.end()) {

                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_point, end_point);
                } else if (foundRoads != roadsOSMID.end() && level_of_detail < 5) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(0.5 * scaleFactor);
                    drawline(start_point, end_point);
                }

            } else {
                //checks what type of street it is and what level_of_detail the user is at
                //draws streets differently according to those conditions


                //draw to - first curve point
                unsigned street_segment_id = segs_connected_to_intersection[j];
                LatLon first_curve_position = street_seg_curve_point [street_segment_id][0];
                vector <double> curve_point_XY = LatLon_to_XY(first_curve_position.lat(), first_curve_position.lon());

                t_point start_coords = t_point(intersection_XY[street_seg_info[segs_connected_to_intersection[j]].from].x, intersection_XY[street_seg_info[segs_connected_to_intersection[j]].from].y);
                t_point end_coords = t_point(curve_point_XY[0], curve_point_XY[1]);

                if (foundResidential != residentialOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundUnclassified != unclassifiedOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundTertiary != tertiaryOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundSecondary != secondaryOSMID.end() && level_of_detail < 10) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(yellowColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundPrimary != primaryOSMID.end() && level_of_detail < 10) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(yellowColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundTrunkLink != trunkLinkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundTrunk != trunkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundMotorwayLink != motorwayLinkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundMotorway != motorwayOSMID.end()) {

                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundRoads != roadsOSMID.end() && level_of_detail < 5) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(0.5 * scaleFactor);
                    drawline(start_coords, end_coords);
                }


                //cout << "Drawn line from: (" <<start_point.x <<"," <<start_point.y << " to: (" <<end_point.x <<"," <<end_point.y <<")." << endl; 

                for (unsigned k = 0; k < (street_seg_info [segs_connected_to_intersection[j]].curvePointCount - 1); k++) {
                    vector <double> start_point_XY = LatLon_to_XY(street_seg_curve_point [segs_connected_to_intersection[j]][k].lat(), street_seg_curve_point [segs_connected_to_intersection[j]][k].lon());
                    vector <double> end_point_XY = LatLon_to_XY(street_seg_curve_point [segs_connected_to_intersection[j]][k + 1].lat(), street_seg_curve_point [segs_connected_to_intersection[j]][k + 1].lon());


                    t_point start_point(start_point_XY[0], start_point_XY[1]);
                    t_point end_point(end_point_XY[0], end_point_XY[1]);

                    if (foundResidential != residentialOSMID.end() && level_of_detail < 7) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(WHITE);
                        setlinewidth(1 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundUnclassified != unclassifiedOSMID.end() && level_of_detail < 7) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(WHITE);
                        setlinewidth(1 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundTertiary != tertiaryOSMID.end() && level_of_detail < 7) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(WHITE);
                        setlinewidth(1 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundSecondary != secondaryOSMID.end() && level_of_detail < 10) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(yellowColor);
                        setlinewidth(2 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundPrimary != primaryOSMID.end() && level_of_detail < 10) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(yellowColor);
                        setlinewidth(2 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundTrunkLink != trunkLinkOSMID.end()) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(orangeColor);
                        setlinewidth(2 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundTrunk != trunkOSMID.end()) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(orangeColor);
                        setlinewidth(2 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundMotorwayLink != motorwayLinkOSMID.end()) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(orangeColor);
                        setlinewidth(2 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundMotorway != motorwayOSMID.end()) {

                        setlinestyle(SOLID, ROUND);
                        setcolor(orangeColor);
                        setlinewidth(2 * scaleFactor);
                        drawline(start_point, end_point);
                    } else if (foundRoads != roadsOSMID.end() && level_of_detail < 5) {
                        setlinestyle(SOLID, ROUND);
                        setcolor(WHITE);
                        setlinewidth(0.5 * scaleFactor);
                        drawline(start_point, end_point);
                    }




                    //cout << "Drawn line from: (" <<start_point.x <<"," <<start_point.y << " to: (" <<end_point.x <<"," <<end_point.y <<")." << endl; 
                }

                //draw last curve point - to curve point
                street_segment_id = segs_connected_to_intersection[j];
                LatLon last_curve_position = street_seg_curve_point [street_segment_id][street_seg_info[street_segment_id].curvePointCount - 1];
                curve_point_XY = LatLon_to_XY(last_curve_position.lat(), last_curve_position.lon());

                start_coords = t_point(curve_point_XY[0], curve_point_XY[1]);
                end_coords = t_point(intersection_XY[street_seg_info[segs_connected_to_intersection[j]].to].x, intersection_XY[street_seg_info[segs_connected_to_intersection[j]].to].y);


                if (foundResidential != residentialOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundUnclassified != unclassifiedOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundTertiary != tertiaryOSMID.end() && level_of_detail < 7) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(1 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundSecondary != secondaryOSMID.end() && level_of_detail < 10) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(yellowColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundPrimary != primaryOSMID.end() && level_of_detail < 10) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(yellowColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundTrunkLink != trunkLinkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundTrunk != trunkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundMotorwayLink != motorwayLinkOSMID.end()) {
                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundMotorway != motorwayOSMID.end()) {

                    setlinestyle(SOLID, ROUND);
                    setcolor(orangeColor);
                    setlinewidth(2 * scaleFactor);
                    drawline(start_coords, end_coords);
                } else if (foundRoads != roadsOSMID.end() && level_of_detail < 5) {

                    setlinestyle(SOLID, ROUND);
                    setcolor(WHITE);
                    setlinewidth(0.5 * scaleFactor);
                    drawline(start_coords, end_coords);
                }

            }
        }
    }


}
//draw street segment 
//draws an individual street segment when given the segment ID and the level of detail
//child of the draw_streets() function

void place_end_marker() {

    LatLon endLatLon = Intersection_LatLon [endNodeID];
    t_point endPoint = LatLon_to_t_point(endLatLon);

    const char* marker_path;
    marker_path = "/nfs/ug/homes-1/z/zhan2860/Documents/ECE297/Milestone1/mapper/libstreetmap/src/easygl/small_image.png";
    Surface POImarker = load_png_from_file(marker_path);
    draw_surface(marker_path, endPoint);
}

void place_start_marker() {

    LatLon startLatLon = Intersection_LatLon [startNodeID];
    t_point startPoint = LatLon_to_t_point(startLatLon);

    const char* marker_path;
    marker_path = "/nfs/ug/homes-1/z/zhan2860/Documents/ECE297/Milestone1/mapper/libstreetmap/src/easygl/small_image.png";
    Surface POImarker = load_png_from_file(marker_path);
    draw_surface(marker_path, startPoint);
}

void draw_street_seg(unsigned seg_ID, int level_of_detail) {
    setcolor(0, 0, 90, 127);
    int scaleFactor = (9 - level_of_detail / 2);
    setlinestyle(SOLID, ROUND);

    setlinewidth(scaleFactor);
    setcolor(0, 0, 75, 75);


    if (street_seg_info [seg_ID].curvePointCount == 0) { //if no curve points on that street segment
        t_point start_point = LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_ID].from]);
        t_point end_point = LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_ID].to]);
        drawline(start_point, end_point);

    } else {
        drawline(
                LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_ID].from]),
                LatLon_to_t_point(street_seg_curve_point[seg_ID][0])
                ); //Adding the distance from from to 1st curve point

        drawline(
                LatLon_to_t_point(street_seg_curve_point[seg_ID][street_seg_info[seg_ID].curvePointCount - 1]),
                LatLon_to_t_point(Intersection_LatLon[street_seg_info[seg_ID].to])
                ); //Adding the distance from the last curve point to to

        for (unsigned i = 0; i < street_seg_info[seg_ID].curvePointCount - 1; i++) {

            drawline(
                    LatLon_to_t_point(street_seg_curve_point[seg_ID][i]),
                    LatLon_to_t_point(street_seg_curve_point[seg_ID][i + 1])
                    ); //Distance between all the curves points. From point and to point are not included inside the curve points
        }





    }
    setcolor(0, 0, 0);

    place_end_marker();
}




//draws all the subway tracks in the city
//Draws subway tracks

void draw_subwayTracks(int zoom_level) {
    int count = 0;

    t_color subwayColor = t_color(217, 142, 113, 200);
    if (zoom_level < 9 && SubwaysOn) {
        for (unordered_map<unsigned, vector < OSMID>>::iterator iter = wayIndex_OSMIDSubwayVector_unordered_map.begin(); iter != wayIndex_OSMIDSubwayVector_unordered_map.end(); iter++) {
            auto OSMVector = iter->second;
            for (unsigned i = 0; i < OSMVector.size() - 1; i++) {
                auto startLatLon = OSMID_LatLon_unordered_map.find(OSMVector[i]);
                auto endLatLon = OSMID_LatLon_unordered_map.find(OSMVector[i + 1]);
                if (startLatLon != OSMID_LatLon_unordered_map.end()) {

                    count++;
                    vector <double> startCoords = LatLon_to_XY(startLatLon->second.lat(), startLatLon->second.lon());
                    vector <double> endCoords = LatLon_to_XY(endLatLon->second.lat(), endLatLon->second.lon());
                    setlinestyle(SOLID, ROUND);
                    setcolor(subwayColor);
                    setlinewidth(3);
                    t_point startpoint = t_point(startCoords[0], startCoords[1]);
                    t_point endpoint = t_point(endCoords[0], endCoords[1]);
                    drawline(startpoint, endpoint);
                }
            }
        }

    }

    setcolor(BLACK);

}

//Finds an intersection after the find button has been pressed

vector<unsigned> find_intersection() {
    string street_name1, street_name2;
    cout << "Enter two street names: " << endl;
    getline(cin, street_name1);
    getline(cin, street_name1);
    getline(cin, street_name2);
    vector <unsigned> intersectionIdsOfStreets = find_intersection_ids_from_street_names(street_name1, street_name2);

    return intersectionIdsOfStreets;
    //    if (intersectionIdsOfStreets.size() == 0) {
    //        cout << street_name1 << " and "<<street_name2<<" do not intersect"<<endl;
    //    } else {
    //        for (unsigned i = 0; i < intersectionIdsOfStreets.size(); i++) {
    //            cout <<"Intersection ID: "<< intersectionIdsOfStreets[i]<<endl;
    //        }
    //    }
}


//Draws markers for POIs
//Toggle-able (needs to be implemented);

void draw_POIs(int level_of_detail) {
    if (level_of_detail < 5 && POIsOn == true) {

        const char* marker_path;
        marker_path = "/nfs/ug/homes-1/z/zhan2860/Documents/ECE297/Milestone1/mapper/libstreetmap/src/easygl/small_image.png"; //REMEMBER TO CHANGE THE IMAGE PATH!
        Surface POImarker = load_png_from_file(marker_path);
        //    
        //     draw_surface(POImarker, coords[0] , coords [2]); 
        //     cout << "Drew POI Marker at: (" << coords[0] << "," << coords[2] << ")" <<endl; 

        for (unsigned i = 0; i < numOfPOIs; i++) {

            draw_surface(POImarker, POI_Data[i].coodinates.x, POI_Data[i].coodinates.y);
            //        cout << "Drew POI Marker at: (" << POIdata[i].x << "," << POIdata[i].y << ")" <<endl; 
        }
    }
}

//Draws features, layered logically.

void draw_features(int level_of_detail) {
    t_color greenColor = t_color(187, 218, 164);
    t_color oceanColor = t_color(155, 191, 244);
    t_color streamColor = t_color(167, 205, 242);
    t_color buildingColor = t_color(192, 192, 192, 125);

    if (WaterOn) {
        setcolor(oceanColor);


        for (unsigned i = 0; i < LakeFeatures.size(); i++) {

            fillpoly(&feature_data[LakeFeatures[i]].boundaryPoints[0], (feature_data[LakeFeatures[i]].boundaryPoints.size()));

        }

    }

    setcolor(232, 221, 151, 255);
    for (unsigned i = 0; i < IslandFeatures.size(); i++) {

        fillpoly(&feature_data[IslandFeatures[i]].boundaryPoints[0], (feature_data[IslandFeatures[i]].boundaryPoints.size()));

    }

    setcolor(173, 162, 138, 255);
    for (unsigned i = 0; i < ShorelineFeatures.size(); i++) {

        fillpoly(&feature_data[ShorelineFeatures[i]].boundaryPoints[0], (feature_data[ShorelineFeatures[i]].boundaryPoints.size()));

    }

    setcolor(219, 219, 103, 255);
    for (unsigned i = 0; i < BeacheFeatures.size(); i++) {

        fillpoly(&feature_data[BeacheFeatures[i]].boundaryPoints[0], (feature_data[BeacheFeatures[i]].boundaryPoints.size()));

    }

    if (GreenOn) {

        setcolor(t_color(greenColor));
        for (unsigned i = 0; i < ParkFeatures.size(); i++) {

            fillpoly(&feature_data[ParkFeatures[i]].boundaryPoints[0], (feature_data[ParkFeatures[i]].boundaryPoints.size()));

        }

        for (unsigned i = 0; i < GreenspaceFeatures.size(); i++) {

            fillpoly(&feature_data[GreenspaceFeatures[i]].boundaryPoints[0], (feature_data[GreenspaceFeatures[i]].boundaryPoints.size()));

        }

        for (unsigned i = 0; i < GolfcourseFeatures.size(); i++) {

            fillpoly(&feature_data[GolfcourseFeatures[i]].boundaryPoints[0], (feature_data[GolfcourseFeatures[i]].boundaryPoints.size()));

        }

    }

    if (WaterOn) {

        setcolor(t_color(141, 176, 240));
        setlinewidth(2);
        for (unsigned i = 0; i < RiverFeatures.size(); i++) {

            fillpoly(&feature_data[RiverFeatures[i]].boundaryPoints[0], (feature_data[RiverFeatures[i]].boundaryPoints.size()));

        }


        if (level_of_detail < 6) {
            setlinewidth(1);
            setcolor(t_color(streamColor));
            for (unsigned i = 0; i < StreamFeatures.size(); i++) {

                for (unsigned k = 0; k < feature_data[StreamFeatures[i]].boundaryPoints.size() - 1; k++) {
                    drawline(feature_data[StreamFeatures[i]].boundaryPoints[k], feature_data[StreamFeatures[i]].boundaryPoints[k + 1]);
                }

            }
        }

    }


    if (level_of_detail < 7 && BuildingOn) {
        setcolor(buildingColor);
        for (unsigned i = 0; i < BuildingFeatures.size(); i++) {

            fillpoly(&feature_data[BuildingFeatures[i]].boundaryPoints[0], (feature_data[BuildingFeatures[i]].boundaryPoints.size()));

        }
    }



    setcolor(0, 0, 0, 255);

}

//finds the boundaries of the map

vector<double> find_map_boundary() {
    /*For every intersection on the map find the min and max LatLon coords. Convert that to Cartesian coords 
     and use as xleft, ybottom, xright, ytop and return vector*/
    double minLat = getIntersectionPosition(0).lat();
    double minLon = getIntersectionPosition(0).lon();

    double maxLat = getIntersectionPosition(0).lat();
    double maxLon = getIntersectionPosition(0).lon();



    for (unsigned i = 0; i < numOfIntersections; i++) {


        minLat = min(minLat, Intersection_LatLon[i].lat());
        maxLat = max(maxLat, Intersection_LatLon[i].lat());

        minLon = min(minLon, Intersection_LatLon[i].lon());
        maxLon = max(maxLon, Intersection_LatLon[i].lon());
    }

    vector <double> minXY = LatLon_to_XY(minLat, minLon);
    vector <double> maxXY = LatLon_to_XY(maxLat, maxLon);

    vector <double> map_boundary;
    map_boundary.push_back(minXY[0]);
    map_boundary.push_back(minXY[1]);
    map_boundary.push_back(maxXY[0]);
    map_boundary.push_back(maxXY[1]);

    return map_boundary;
}

//this function returns a vector<double> where the four elements in
//the vector are leftX, rightX, bottomY, topY IN THAT ORDER

vector<double> latLon_to_cartesian(double min_Lat, double max_Lat, double min_Lon, double max_Lon) {
    average_lat = ((min_Lat + max_Lat) / 2) * DEG_TO_RAD;


    double leftX = XStretchFactor * min_Lon * DEG_TO_RAD * cos(average_lat) * EARTH_RADIUS_IN_METERS / 1000;
    double rightX = XStretchFactor * max_Lon * DEG_TO_RAD * cos(average_lat) * EARTH_RADIUS_IN_METERS / 1000;

    double bottomY = YStretchFactor * min_Lat * DEG_TO_RAD * EARTH_RADIUS_IN_METERS / 1000;
    double topY = YStretchFactor * max_Lat * DEG_TO_RAD * EARTH_RADIUS_IN_METERS / 1000;

    vector<double> catesian;

    catesian.push_back(leftX);
    catesian.push_back(rightX);
    catesian.push_back(bottomY);
    catesian.push_back(topY);

    return catesian;
}

//set of conversion functions to go from LatLon to XY and back
//Function to return the xy coordinates form a single LatLon point
//return vector is X then Y

vector<double> LatLon_to_XY(double Lat, double Lon) {

    vector<double> XY;

    double X = -XStretchFactor * ((Lon * cos(average_lat)) * DEG_TO_RAD * EARTH_RADIUS_IN_METERS) / 1000;
    double Y = YStretchFactor * Lat * DEG_TO_RAD * EARTH_RADIUS_IN_METERS / 1000;

    XY.push_back(X);
    XY.push_back(Y);

    return XY;
}
//converts LatLon to x-y coordinates and returns a t_point

t_point LatLon_to_t_point(LatLon position) {
    double X = -XStretchFactor * (position.lon() * cos(average_lat)) * DEG_TO_RAD * EARTH_RADIUS_IN_METERS / 1000;
    double Y = YStretchFactor * position.lat() * DEG_TO_RAD * EARTH_RADIUS_IN_METERS / 1000;

    return t_point(X, Y);
}
//converts x-y coordinates to lat and lon

LatLon XY_to_LatLon(float x, float y) {

    LatLon retVal(y / EARTH_RADIUS_IN_METERS * 1000 / DEG_TO_RAD / YStretchFactor, x / -(EARTH_RADIUS_IN_METERS)*1000 / cos(average_lat) / DEG_TO_RAD / XStretchFactor);

    return retVal;

}

//Returns current level of detail
//Each level is twice as zoomed in -> displays 1/4 of the area

int current_level_of_detail() {

    //double EARTH_RADIUS_IN_KM = EARTH_RADIUS_IN_METERS / 1000;
    int LOD;
    km_per_char = find_km_per_char();


    if (LOD_area_test(0.0036 * XStretchFactor * YStretchFactor)) {
        LOD = 0; //unit length 5m

        /*Shown (Includes Previous Zoom Level elements):
         Streets: 
         Feature: Everything
         POIs:    Everything*/
    } else

        if (LOD_area_test(0.0144 * XStretchFactor * YStretchFactor)) {
        LOD = 1; //unit length 10m
        /*Shown (Includes Previous Zoom Level elements):
         Streets: Everything
         Feature: Everything
         POIs:    Very Minor POIs*/
    } else if (LOD_area_test(0.0576 * XStretchFactor * YStretchFactor)) {
        LOD = 2; //unit length 20m
        /*Shown (Includes Previous Zoom Level elements):
         Streets: pedestrian, road, footway, paths
         Feature: Buildings
         POIs:    Everything else*/
    } else if (LOD_area_test(0.2304 * XStretchFactor * YStretchFactor)) {
        LOD = 3; //unit length 40m
        /*Shown (Includes Previous Zoom Level elements):
         Streets: Living street
         Feature: Everything else
         POIs:    Shops*/
    } else if (LOD_area_test(0.9216 * XStretchFactor * YStretchFactor)) {
        LOD = 4; //unit length 80m
        /*Shown (Includes Previous Zoom Level elements):
         Streets: Service
         Feature: 
         POIs:    */
    } else if (LOD_area_test(3.6864 * XStretchFactor * YStretchFactor)) {
        LOD = 5; //unit length 160m
        /*Shown (Includes Previous Zoom Level elements):
         Streets: Residential
         Feature: 
         POIs:   Tourist attractions*/
    } else if (LOD_area_test(14.7456 * XStretchFactor * YStretchFactor)) {
        LOD = 6; //unit length 320m
        /*Shown (Includes Previous Zoom Level elements):
         Streets: 
         Feature: Everything else
         POIs:   */
    } else if (LOD_area_test(58.9824 * XStretchFactor * YStretchFactor)) {
        LOD = 7; //unit length 640m 10.24km x 5.76km
        /*Shown (Includes Previous Zoom Level elements):
         Streets: Tertiary and Links, unclassified
         Feature: 
         POIs:    Amenity*/
    } else if (LOD_area_test(235.9296 * XStretchFactor * YStretchFactor)) {
        LOD = 8; //unit length 1280m 20.48km x 11.52km
        /*Shown (Includes Previous Zoom Level elements):
         Streets: Secondary and Links
         Feature: Shorelines, Beaches
         POIs:    */
    } else if (LOD_area_test(943.7184 * XStretchFactor * YStretchFactor)) {
        LOD = 9; //unit length 2560m 40.96km x 23.04km
        /*Shown (Includes Previous Zoom Level elements):
         Streets: Primary and Links
         Feature: Golf courses
         POIs: */
    } else if (LOD_area_test(3774.8736 * XStretchFactor * YStretchFactor)) {
        LOD = 10; //unit length 5120m 81.92km x 46.08km
        /*Shown (Includes Previous Zoom Level elements):
         Streets: 
         Feature: 
         POIs:    */
    } else LOD = 11; //at any zoom level
    /*Shown (Includes Previous Zoom Level elements):
     Streets: Motorways and Links, Trunks and Links, sidewalks
     Feature: Lakes, Rivers, Islands, Parks, Unknowns
     POIs:Historic, LandForms, Water Related*/


    //cout <<"Current LOD: " << LOD <<endl;

    return LOD;
}

float distance_between_two_t_points(t_point point1, t_point point2) {

    return (sqrt(pow((point1.x - point2.x), 2) + pow((point1.y - point2.y), 2)));
}

//before calling this function makes sure that the curve is long enough to fit all the characters
//unused 

void draw_string_along_points(vector<t_point> the_curve, float km_per_character_on_screen, string name) {
    size_t length = name.length();
    size_t position = 0;
    size_t vector_position = 0;
    unsigned number_of_char;
    string partial_string;
    t_point centreOfText;
    float deltaX, deltaY, rotation;

    while (position < length) {
        number_of_char = ceil(distance_between_two_t_points(the_curve[vector_position], the_curve[vector_position]) / km_per_character_on_screen);
        partial_string = name.substr(position, number_of_char);
        position += number_of_char;

        deltaX = the_curve[vector_position].x + the_curve[vector_position + 1].x;
        deltaY = the_curve[vector_position].y + the_curve[vector_position + 1].y;

        if (abs(deltaX) < 0.00000001) {
            rotation = -90;
            settextrotation(rotation);

        } else {//if delta x is smaller than 10e-8 and when the scale of we are using is around 10e-4, we may overflow atan which takes in a double

            rotation = -atan(deltaY / deltaX);
            rotation = rotation * 180 / PI;
            settextrotation(rotation);

        }

        centreOfText.x = the_curve[vector_position].x + ceil(partial_string.length() / 2) * km_per_character_on_screen * cos(rotation / 180 * PI);
        centreOfText.y = the_curve[vector_position].y + ceil(partial_string.length() / 2) * km_per_character_on_screen * sin(rotation / 180 * PI);
        drawtext(centreOfText.x, centreOfText.y, partial_string);
    }


}

float find_km_per_char() {

    return (abs(get_visible_world().right() - get_visible_world().left()) / 210);
}
//places marker for POIs

void place_marker(float x, float y, int LOD) {

    double scale = pow(1.75, LOD);

    t_point points[4];
    t_point shadow[4];

    //Marker points
    t_point click_point(x, y);
    t_point left(x + ((.002) * scale), y + ((.003) * scale));
    t_point right(x - (.002 * scale), y + (.003 * scale));
    t_point top(x, y + (.006 * scale));

    points[0] = click_point;
    points[3] = left;
    points[1] = right;
    points[2] = top;

    //shadow points
    t_point click_point_SH(x, y);
    t_point left_SH(x + ((.001) * scale), y + ((.003) * scale));
    t_point right_SH(x - (.003 * scale), y + (.00266 * scale));
    t_point top_SH(x - (.002 * scale), y + (.006 * scale));

    shadow[0] = click_point_SH;
    shadow[3] = left_SH;
    shadow[1] = right_SH;
    shadow[2] = top_SH;

    setcolor(0, 0, 0, 31);
    fillpoly(shadow, 4);

    setcolor(186, 40, 35, 215);
    fillpoly(points, 4);


}

void place_Dirmarker(float x, float y, int LOD) {

    double scale = pow(1.75, LOD);

    t_point points[4];
    t_point shadow[4];

    //Marker points
    t_point click_point(x, y);
    t_point left(x + ((.002) * scale), y + ((.003) * scale));
    t_point right(x - (.002 * scale), y + (.003 * scale));
    t_point top(x, y + (.006 * scale));

    points[0] = click_point;
    points[3] = left;
    points[1] = right;
    points[2] = top;

    //shadow points
    t_point click_point_SH(x, y);
    t_point left_SH(x + ((.001) * scale), y + ((.003) * scale));
    t_point right_SH(x - (.003 * scale), y + (.00266 * scale));
    t_point top_SH(x - (.002 * scale), y + (.006 * scale));

    shadow[0] = click_point_SH;
    shadow[3] = left_SH;
    shadow[1] = right_SH;
    shadow[2] = top_SH;


    fillpoly(points, 4);


    setcolor(0, 0, 0, 31);
    fillpoly(shadow, 4);



}

void draw_intersection_text_box(float x, float y, int LOD) {

    double scale = pow(1.75, LOD);

    setcolor(252, 250, 249, 200);

    fillrect(x + (0.03 * scale), y + (0.006 * scale), x - (0.03 * scale), y + (0.012 * scale));

    setcolor(0, 0, 0, 255);

    settextrotation(0);

    string outputData;

    LatLon mouse_pos = XY_to_LatLon(x, y);

    unsigned nearest_int = find_closest_intersection(mouse_pos);

    outputData = getIntersectionName(nearest_int);
    //outputData = to_string(nearest_int); //show index
    drawtext(x, y + (0.009 * scale), outputData);


}

void draw_POI_text_box(float x, float y, int LOD) {

    double scale = pow(1.75, LOD);

    setcolor(252, 250, 249, 200);

    fillrect(x + (0.03 * scale), y + (0.004 * scale), x - (0.03 * scale), y + (0.008 * scale));

    setcolor(0, 0, 0, 255);

    settextrotation(0);

    string outputData;

    LatLon mouse_pos = XY_to_LatLon(x, y);

    unsigned nearest_POI = find_closest_point_of_interest(mouse_pos);

    setfontsize(12);

    outputData = getPointOfInterestType(nearest_POI) + ":       " + getPointOfInterestName(nearest_POI);


    drawtext(x, y + (0.006 * scale), outputData);


}

void draw_path(std::vector<unsigned> Segs) {

    for (unsigned i = 0; i < Segs.size(); i++) {

        draw_street_seg(Segs[i], 0);
    }
}





//
//void delay(long milliseconds) {
//  
//    std::chrono::milliseconds duration(milliseconds);
//    std::this_thread::sleep_for(duration);
//}
//
//void mouseClickAni(float x, float y) {
//
//    set_drawing_buffer(ON_SCREEN);
//    setcolor(t_color(255, 0, 0, 100));
//    long delayTime = 300;
//    
//    for (int i = 16; i >= 0; i--) {
//        
//        draw_screen();
//        drawarc(x, y, i, 0, 360);
//        delay(delayTime);
//        
//        
//    }    
//    set_drawing_buffer(OFF_SCREEN);
//
//
//}


//Unused

//Initial loading screen call for input
//
//void draw_initialLoadingScreen() {
//
//    set_coordinate_system(GL_SCREEN);
//    setcolor(t_color(252, 250, 249, 255));
//    fillrect(coords[0], coords[1], coords [2], coords[3]);
//    if (findTextOff == true) drawtext(500, 500, "Enter an new map name (\"city_country\" or just \"country\" for small states): ");
//
//    set_coordinate_system(GL_WORLD);
//
//}
//


////Delays the map update by the specified number of frames
//
//void delay(int frames) {
//
//    for (int i = 0; i <= frames; i++) {
//        draw_screen();
//    }
//
//}

//Bonus Feature

//t_point* gen_shadow_points(t_point* original_pts, int numOfPoints) {
//
//
//}

//OLD LOD SET
//int LOD = 0; //Default -> Zoomed Out.
/*Shown:
 Streets: Motorway, Motorway Link, Trunk, Trunk link
 Feature: Parks,  Lakes, Rivers, Island
 POIs: Historic, LandForms, Water Related*/

//if (LOD_area_test(.086891866*EARTH_RADIUS_IN_KM*EARTH_RADIUS_IN_KM)) LOD = 1; // Zoom Level 1
/*Shown (Includes Previous Zoom Level elements):
 Streets: Primary, PrimaryLink, Secondary, Secondary link
 Feature: GolfCourses
 POIs: */

//if (LOD_area_test(.021722966*EARTH_RADIUS_IN_KM*EARTH_RADIUS_IN_KM)) LOD = 2; // Zoom Level 2
/*Shown (Includes Previous Zoom Level elements):
 Streets: Tertiary, Tertiary Link, Unclassified
 Feature: Shoreline, Beaches
 POIs: */


//if (LOD_area_test(.005430741*EARTH_RADIUS_IN_KM*EARTH_RADIUS_IN_KM)) LOD = 3; // Zoom Level 3
/*Shown (Includes Previous Zoom Level elements):
 Streets: Living streets, Roads
 Feature: Streams, Green space
 POIs: */

//if (LOD_area_test(.001357685*EARTH_RADIUS_IN_KM*EARTH_RADIUS_IN_KM)) LOD = 4; // Zoom Level 4 (MAX!)
/*Shown (Includes Previous Zoom Level elements):
 Streets: Everything else
 Feature: Everything else
 POIs:    Everything else*/


//NEW LOD SCALE, BOTTOM UP, 0 is most detail up to 10+ since the screen is 16x9 = 144 unit length^2

