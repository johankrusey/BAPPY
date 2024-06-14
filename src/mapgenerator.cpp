#include "mapgenerator.h"
#include "load_flow.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void MapGenerator::_bind_methods() { // When the time_index_signal is emitted by the Timebase node, the _on_timebase_time_index_signal method of the MapGenerator node will be called.
    ClassDB::bind_method(D_METHOD("_on_timebase_time_index_signal"), &MapGenerator::_on_timebase_time_index_signal);

    ClassDB::bind_method(D_METHOD("get_wind"), &MapGenerator::get_wind);
    ClassDB::bind_method(D_METHOD("set_wind", "p_wind"), &MapGenerator::set_wind);
    ClassDB::add_property("MapGenerator", PropertyInfo(Variant::FLOAT, "Wind Power (p.u.)"), "set_wind", "get_wind");

    ClassDB::bind_method(D_METHOD("get_solar"), &MapGenerator::get_solar);
    ClassDB::bind_method(D_METHOD("set_solar", "p_sized"), &MapGenerator::set_solar);
    ClassDB::add_property("MapGenerator", PropertyInfo(Variant::FLOAT, "Solar Power (p.u.)"), "set_solar", "get_solar");
} // You can also define more slider-like properties such as image size where you can increase and decrease it in godot, for example, in this function _bind_methods, for an example with path movement look at the "Adding properties" part of: https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html

MapGenerator::MapGenerator() : tiles(nullptr), linenode(nullptr) {
    wind = 1.0;
    solar = 1.0;
}

MapGenerator::~MapGenerator() { // To prevent data leaks when closing godot you have to delete everything in the destructor
    // Check if nodes are inside the tree before trying to remove them
    if (linenode) {
        if (linenode->is_inside_tree()) { // Find the node children and then delete them
            remove_child(linenode);
        }
        memdelete(linenode);
        linenode = nullptr;
    }
    if (tiles) {
        if (tiles->is_inside_tree()) {
            remove_child(tiles); // Find the node children and then delete them
        }
        memdelete(tiles);
        tiles = nullptr;
    }
}


void MapGenerator::_ready() { // This function is called directly after the constructor
    dothepowerthing(0, index, solar, wind); // This does the power calculations based on the file load_flow.hpp, 0 is the scenario and index is the timestamp, which is always 0 here

    tiles = memnew(Tileset); // Here memory is allocated for the tilemap
    add_child(tiles);   // Add them as a child to the main node
    tiles->_create_map(); // Here the map is created via a function defined in tileset.cpp

    linenode = memnew(Powerline); // Here memory is allocated for the lines
    linenode->_create_powerlines(lines, rowcount); // Here the lines are created via the function defined in powerline.cpp
    add_child(linenode); // Add them as a child to the main node
    
    Node* parentnode = get_parent();
    Timer* timenode = parentnode->get_node<Timer>("Timebase");
    if (timenode){
        timenode->connect("time_index_signal", Callable(this, "_on_timebase_time_index_signal"));
    }
    else{
        UtilityFunctions::print("Timebase node not found");
    }
}

float MapGenerator::get_solar() const { //This allows the function to obtain the current value of solar power
    return solar;
}
float MapGenerator::get_wind() const { //This allows the function to obtain the current value of wind power
    return wind;
}
void MapGenerator::set_wind(const float p_wind) { //This allows the setting of wind power via Godot via a function
    if (p_wind != wind) {
        wind = p_wind;
    }
    return;
}
void MapGenerator::set_solar(const float p_solar) { //This allows the setting of solar power via Godot via a function
    if (p_solar != solar) {
        solar = p_solar;
    }
    return;
}

void MapGenerator::_on_timebase_time_index_signal(int index) { // When this function is called the next timestep will have been reached and the lines will be destructed
    if (linenode) {
        if (linenode->is_inside_tree()) {
            remove_child(linenode);
        }
        memdelete(linenode);
        linenode = nullptr;
    }
    dothepowerthing(0, index, solar, wind); // After everything is gone calculate the power flow in the new time instance will run
    linenode = memnew(Powerline); // The new lines will be generated to correspond with the new calculations
    linenode->_create_powerlines(lines, rowcount); // This might alter their color or arrow direction
    add_child(linenode); // Add the new lines as a child to the main node
    return;
}

void MapGenerator::_print_tree(){
    SceneTree* tree = get_tree();
        if (tree) {
            Node* root = tree->get_root();
            if (root) {
                _print_node_tree(root, 0);
            }
        }
    return;
}

void MapGenerator::_print_node_tree(Node* node, int depth){
    nodename = node->get_name();
    for (int i = 0; i < depth; ++i) {
            cout << "    ";
        }
    cout << nodename.utf8().get_data() << "\n";

    int child_count = node->get_child_count();
    for (int i = 0; i < child_count; ++i) {
        _print_node_tree(node->get_child(i), depth + 1);
    }
    return;
}
