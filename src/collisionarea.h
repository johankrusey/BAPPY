#ifndef COLLISIONAREA_H
#define COLLISIONAREA_H

#include "canvascontrol.h"
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>

namespace godot {

    class CollisionArea : public Area2D {
        GDCLASS(CollisionArea, Area2D)

    private:
        CollisionShape2D* shape;
        RectangleShape2D* square;
        SceneTree* tree;
        int xpos;
        int ypos;
    protected:
        static void _bind_methods();

    public:
        CanvasControl* controlnode;
        CollisionArea();
        CollisionArea(int, int);
        ~CollisionArea();

        void _ready();
        void _mouse_enter();
        void _mouse_exit();
    };

} // namespace godot

#endif // GRIDSET_H
