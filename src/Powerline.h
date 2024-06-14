#ifndef POWERLINE_H
#define POWERLINE_H

#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot {

    class Powerline : public Node2D { //Create a custom class called Powerline based on the godot Node2D class
        GDCLASS(Powerline, Node2D) //For the commands and documentation of Node2D you can look at: https://docs.godotengine.org/en/stable/classes/class_node2d.html
    
    private:
        Line2D* line; //This class will call multiple Line2D classes as children, for commands and documentation of that you can look at: https://docs.godotengine.org/en/stable/classes/class_line2d.html
        void _cleanup_powerlines();//This initializes the function that cleans up the lines

    protected:
        static void _bind_methods(); //Initialize the bind method functions, explained in the powerline.cpp file

    public:
        Powerline();
        ~Powerline();

        void _create_powerlines(int arr[][4], int rowcount); //Initiate the function that will create the lines
    };

}

#endif 