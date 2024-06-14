#include "tileset.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/tile_map.hpp>
#include <string.h>

using namespace godot;

void Tileset::_bind_methods() {
    // No properties to bind
}

void Tileset::_create_map() {
    for (int i = 0; i < 195; i++) {
        for (int j = 0; j < 119; j++) {
            if (Map[i][j] != ""){
                PackedStringArray tileinfo = Map[i][j].split(",");
                tiletype = tileinfo[0].to_int();
                tilenumber = tileinfo[1].to_int();
            }
            else{
                tiletype = 0;
                tilenumber = 0;
            }
            switch (tiletype) {
            case 1: // Wind generator
                set_cell(0, Vector2i(i, j), 0, Vector2i(0, 0), 0);
                collisionbox = memnew(CollisionArea(i, j));
                add_child(collisionbox);
                break;
            case 4: // Industry
                set_cell(0, Vector2i(i, j), 0, Vector2i(0, 1), 0);
                collisionbox = memnew(CollisionArea(i, j));
                add_child(collisionbox);
                break;
            case 3: // City
                set_cell(0, Vector2i(i, j), 0, Vector2i(0, 3), 0);
                collisionbox = memnew(CollisionArea(i, j));
                add_child(collisionbox);
                break;
            case 2: // Solar generator
                set_cell(0, Vector2i(i, j), 0, Vector2i(0, 2), 0);
                collisionbox = memnew(CollisionArea(i, j));
                add_child(collisionbox);
                break;
            case 5: // Nuclear generator
                set_cell(0, Vector2i(i, j), 0, Vector2i(0, 5), 0);
                collisionbox = memnew(CollisionArea(i, j));
                add_child(collisionbox);
                break;
            case 6: // Gas generator
                set_cell(0, Vector2i(i, j), 0, Vector2i(0, 4), 0);
                collisionbox = memnew(CollisionArea(i, j));
                add_child(collisionbox);
                break;
            case 7: // Slack bus
                set_cell(0, Vector2i(i, j), 0, Vector2i(0, 6), 0);
                collisionbox = memnew(CollisionArea(i, j));
                add_child(collisionbox);
                break;
            default: // Do nothing for the other tiles
                break;
            }
        }
    }
}

void Tileset::_ready(){
}

int Tileset::_fetch_node_index(int xloc, int yloc){
    PackedStringArray temp_string = Map[xloc][yloc].split(",");
    int temp1 = temp_string[1].to_int();
    return temp1;
}

Tileset::Tileset() {
    tileset = ResourceLoader::get_singleton()->load("res://assets/6maalepisch.tres");
    if (!tileset.is_valid()) {
        UtilityFunctions::printerr("Failed to load TileSet resource.");
        return;
    }
    set_tileset(tileset);
    set("cell_size", Vector2(16,16));
    set_z_index(1);

    for (int i = 0; i < 195; i++) {
        for (int j = 0; j < 119; j++) {
            Map[i][j] = "";
        }
    }

    // Here you define every bus into a position on the x and y grid
    // When changing busses, remember to also update solar and wind bus numbers in load_flow.hpp
    Map[88][64] = "7,1"; // 1 Main slack meest links ZH 
    Map[128][37] = "6,2"; // 2 Puntje overijsel***
    Map[137][41] = "6,3"; // 3 Overijsel ***
    Map[127][18] = "3,4"; // 4 Leeuwarden
    Map[154][14] = "6,5"; // 5 Groningen midden ***
    Map[128][25] = "6,6"; // 6 Friesland onder ***
    Map[148][9] = "6,7"; // 7 Groningen massa node***
    Map[137][10] = "6,8"; // 8 Groningen links ***
    Map[151][13] = "6,9"; // 9 Groningen midden ***
    Map[142][24] = "3,10"; // 10 Assen 
    Map[111][53] = "3,11"; // 11 Utrecht 
    Map[83][77] = "5,12"; // 12 Zeeland left 
    Map[102][44] = "3,13"; // 13 Haarlem 
    Map[101][56] = "6,14"; // 14 Spits ZH ***
    Map[97][64] = "3,15"; // 15 Rotterdam
    Map[111][47] = "3,16"; // 16 Amsterdam
    Map[125][64] = "3,17"; // 17 Nijmwegen
    Map[135][59] = "3,18"; // 18 Arnhem
    Map[145][15] = "3,19"; // 19 Groningen 
    Map[121][76] = "3,20"; // 20 Eindhoven
    Map[129][44] = "3,21"; // 21 Apeldoorn
    Map[147][13] = "6,22"; // 22 Midden boven groningen ***
    Map[105][68] = "6,23"; // 23 rechtst spits ZH ***
    Map[146][51] = "3,24"; // 24 Enschede
    Map[102][61] = "6,25"; // 25 rechts spits ZH ***
    Map[121][39] = "3,26"; // 26 Almere of lelystad ofzo
    Map[130][86] = "3,27"; // 27 Roermond
    Map[147][24] = "6,28"; // 28 Groningen ***
    Map[90][59] = "6,29"; // 29 links spits ZH ***
    Map[107][45] = "6,30"; // 30 NH links +2 ***
    Map[91][77] = "6,31"; // 31 Zeeland middle ***
    Map[102][47] = "6,32"; // 32 NH links +1 ***
    Map[95][58] = "3,33"; // 33 Den Haag
    Map[97][56] = "6,34"; // 34 Links spits ZH ***
    Map[147][42] = "7,35"; //35 Duitsland overlap bus 
    Map[143][12] = "6,36"; // 36 Groningen above ***
    Map[150][16] = "7,37"; // 37 Groningen ook suprise 
    Map[147][18] = "7,38"; // 38 Groningen ook midden maar meer rechts ***
    Map[156][21] = "7,39"; // 39 Groningen germany grens 
    Map[151][52] = "7,40"; // 40 Germany noord brabant boven 
    Map[141][62] = "7,41"; // 41 ANB bottom germany 
    Map[134][88] = "7,42"; // 42 Limburg grens duitsland onder 
    Map[135][81] = "7,43"; // 43 limburg grens boven 
    Map[128][89] = "7,44"; // 44 Near Maastricht border on the left 
    Map[94][80] = "3,45"; // 45 Antwerpen
    Map[88][31] = "1,46"; //46 Windgen 
    Map[125][9] = "2,47"; //47 Solar
}

Tileset::~Tileset() {
    tileset.unref();
}