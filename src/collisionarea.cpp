#include "collisionarea.h"
#include "canvascontrol.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/control.hpp>

using namespace godot;

void CollisionArea::_bind_methods() { 
}

CollisionArea::CollisionArea(){}

CollisionArea::CollisionArea(int xloc, int yloc){
    xpos = xloc;
    ypos = yloc;
    CollisionShape2D* shape = new CollisionShape2D;
    RectangleShape2D* square = new RectangleShape2D;
    square->set_size(Vector2(16,16));
    set_pickable(true);
    set_collision_layer(true);
    translate(Vector2(8 + 16*xpos, 8 + 16*ypos));
    shape->set_shape(square);
    add_child(shape);
}

CollisionArea::~CollisionArea(){

}

void CollisionArea::_ready(){
}

void CollisionArea::_mouse_enter(){
    UtilityFunctions::print("mouse entered");
    SceneTree* tree = get_tree();
    if (tree) {
        Node* root = tree->get_root();
        if (root){
            Node* scenenode = root->get_child(0);
            if (scenenode) {
                CanvasControl* controlnode = scenenode->get_node<CanvasControl>("CanvasControl");
                if (controlnode){
                    UtilityFunctions::print("trying to create popup");
                    controlnode->_create_popup(xpos, ypos);
                }
                else{
                    UtilityFunctions::print("failed to find control node for popup");
                }
            }
            
        }
    }
    
    return;
}

void CollisionArea::_mouse_exit(){
    UtilityFunctions::print("mouse exited");
    SceneTree* tree = get_tree();
    if (tree) {
        Node* root = tree->get_root();
        if (root){
            CanvasControl* controlnode = root->get_node<CanvasControl>("Node2D/CanvasControl");
            if (controlnode){
                UtilityFunctions::print("trying to delete popup");
                controlnode->_delete_popup();
            }
            else {
                UtilityFunctions::print("failed to delete popup");
            }       
        }
    }
    
    return;
}
