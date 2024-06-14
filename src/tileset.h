#ifndef TILESET_H
#define TILESET_H

#include "collisionarea.h"
#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/classes/tile_set.hpp>
#include <godot_cpp/classes/tile_set_source.hpp>
#include <godot_cpp/classes/control.hpp>
#include <string>


namespace godot {

    class Tileset : public TileMap { //Create a custom class called Tileset in based on the godot tilemap class
        GDCLASS(Tileset, TileMap) //For the commands and documentation you can look at: https://docs.godotengine.org/en/stable/classes/class_tilemap.html#class-tilemap

    private:
        Ref<TileSet> tileset;
        CollisionArea* collisionbox; //Initialize the collisionbox pointer to show the informationboxes ingame
        Control* canvascontrol;
        String Map[196][120] = {""}; //Initialize the map size, then also change these variables in the tileset.cpp file
        int tiletype;
        int tilenumber;
    protected:
        static void _bind_methods();//Initialize the bind method functions, explained in the tileset.cpp file

    public:
        Tileset();
        ~Tileset();

        void _ready();
        int _fetch_node_index(int xloc, int yloc);
        void _create_map(); // Initialize functions specified in the tileset.cpp file
        void _create_collider(int xloc, int yloc); //Initiate the create collider function, specified in the tileset.cpp file
    };

} // namespace godot

#endif // GRIDSET_H
