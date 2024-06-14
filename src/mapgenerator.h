#ifndef MAPGENERATION_H
#define MAPGENERATION_H

#include "powerline.h"
#include "tileset.h"
#include "timebase.h"
#include "canvascontrol.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/string.hpp>
#include <iostream>
//This is the top-level class that implements all the other nodes as children
//It generates the tiles in the tilemap via tileset.h
//It generates all the lines via powerline.h
//It keeps track of time progression via timebase.h
//The one thing it does not do it place the background image, but you can do that manually in a godot scene

namespace godot { //!!!!!If you opened this file for the first time and the program does not work, make sure you have the 5 hardcoded paths in the powerline.cpp and load_flow.hpp files set to the ones corresponding to your own setup

class MapGenerator : public Node {
    GDCLASS(MapGenerator, Node)

private: //Define all the lines here, x1,y1,x2,y2 , this has to be an array of the form [][4] otherwise your compiler will cry
    //For defining the busses, their locations or their numbers for power flow look at tileset.cpp
    static constexpr int rowcount =  54;
    float wind, solar;
    int lines[rowcount][4] = {
        {128, 89, 130, 86}, // line 1 44-27
        {137, 41, 128, 37}, // line 2 3-2
        {127, 18, 137, 10}, // line 3 4-8
        {127, 18, 128, 25}, // line 4 4-6
        {127, 18, 125, 9}, // line 5 4-47
        {128, 25, 128, 37}, // line 6 6-2
        {148, 9, 151, 13}, // line 7 7-9
        {137, 10, 143, 12}, // line 8 8-36
        {137, 10, 148, 9}, // line 9 8-7
        {137, 10, 142, 24}, // line 10 8-10
        {151, 13, 154, 14}, // line 11 9-5
        {142, 24, 137, 41}, // line 12 10-3
        {111, 53, 111, 47}, // line 13 11-16
        {83, 77, 91, 77}, // line 14 12-31
        {102, 44, 107, 45}, // line 15 13-30
        {102, 44, 102, 47}, // line 16 13-32
        {102, 44, 88, 31}, // line 17 13-46
        {101, 56, 102, 47},  // line 18 14-32
        {97, 64, 102, 61},  // line 19 15-25
        {111, 47, 121, 39},   // line 20 16-26
        {125, 64, 135, 59},   // line 21 17-18
        {135, 59, 146, 51}, // line 22 18-24
        {135, 59, 141, 62}, // line 23 18-41
        {145, 15, 147, 24}, // line 24 19-28
        {129, 44, 147, 42}, // line 25 21-35 
        {147, 13, 145, 15}, // line 26 22-19
        {105, 68, 121, 76}, // line 27 23-20
        {105, 68, 91, 77}, // line 28 23-31
        {102, 61, 111, 53}, // line 29 25-11
        {102, 61, 101, 56}, // line 30 25-14
        {102, 61, 105, 68}, // line 31 25-23
        {102, 61, 107, 45}, // line 32 25-30
        {121, 39, 129, 44}, // line 33 26-21 
        {130, 86, 125, 64}, // line 34 27-17
        {130, 86, 121, 76}, // line 35 27-20
        {130, 86, 134, 88}, // line 36 27-42
        {130, 86, 135, 81}, // line 37 27-43 
        {147, 24, 156, 21},  // line 38 28-39 
        {90, 59, 97, 64},  // line 39 29-15
        {107, 45, 111, 47},   // line 40 30-16
        {91, 77, 94, 80},   // line 41 31-45
        {95, 58, 90, 59 }, // line 42 33-29
        {97, 56, 101, 56}, // line 43 34-14
        {97, 56, 95, 58}, // line 44 34-33
        {147, 42, 146, 51}, // line 45 35-24
        {147, 42, 147, 24}, // line 46 35-28
        {88, 64, 90, 59}, // line 47 1-29
        {150, 16, 147, 13},  // line 48 37-22
        {147, 18, 145, 15},  // line 49 38-19
        {151, 52, 146, 51},   // line 50 40-24
        {143, 12, 148, 9},   // line 51 36-7
        {145,15,143,12}, // line 52/transformer 19-36 
        {129,44,128,37}, // line 53/transformer 21-2 
        { 147,24,154,14 } // line 54/transformer 28-5 
    };

    Tileset* tiles; //This allows it to call tilesets (multiple in theory, but only one needed) as children
    Powerline* linenode; //This allows it to call powerlines (multiple in theory, but only one needed) as children
    int index = 0; //Initialize the index integer, further specified in mapgenerator.cpp
    int map[196][120] = { 0 }; //Initialize the map array, further specified in mapgenerator.cpp
    String nodename; //String used to print the entire scenetree, used for troubleshooting
protected:
    static void _bind_methods(); //Initialize the bind method functions, explained in the mapgenerator.cpp file
    
public:
    MapGenerator();
    ~MapGenerator();

    void set_wind(const float p_wind);
    float get_wind() const;

    void set_solar(const float p_solar);
    float get_solar() const;

    void _ready();//Initialize the _ready function to be called directly after the constructor
    void _on_timebase_time_index_signal(int index); 
    void _print_tree();
    void _print_node_tree(Node* node, int depth);
};
}

#endif
