
#include "DataStructures.h"
#include <iostream>
#include <functional>
#include <string>
#include "m2.h"
#include "m3.h"
#include "m1.h"
#include "m2Mod.h"
#include "m3Mod.h"
#include "Global_Variables.h"
using namespace std;

int main(int argc, char** argv) {

    std::string map_path;

    while (!endProgram) {
            //Load the map and related data structures

            endProgram = false;
            bool load_success = UserLoadMap();

            if (!load_success) {
                std::cerr << "Failed to load map " << "\n" << endl;
            }
            
            else if (endProgram){
                break;
            }
            
            
            else {
                std::cout << "Successfully loaded map " << "\n";

                draw_map();

                std::cout << "Closing map\n";
                

                close_map();
            }
            
            


        //    //Debug testing
        //    bool load_success = false;
        //    {
        //        load_success = load_map("/cad2/ece297s/public/maps/beijing_china.streets.bin");
        //
        //    }




    }
    
    
    cout << "Exiting...." << endl;

    close_graphics();

    return 0;
}
