#include "timebase.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


using namespace godot;

void Timebase::_bind_methods() { //Create the sized property (as size is already a thing) to depict the size of the whole grid
    ClassDB::bind_method(D_METHOD("set_timer_amount", "new_amount"), &Timebase::set_timer_amount);
    ClassDB::bind_method(D_METHOD("get_timer_amount"), &Timebase::get_timer_amount);
    ClassDB::bind_method(D_METHOD("_on_timebase_timeout"), &Timebase::_on_timebase_timeout);
    ClassDB::bind_method(D_METHOD("_on_direction_signal"), &Timebase::_on_direction_signal);
    ClassDB::add_property("Timebase",PropertyInfo(Variant::FLOAT,"amount"), "set_timer_amount", "get_timer_amount"); 
    ADD_SIGNAL(MethodInfo("time_index_signal", PropertyInfo(Variant::INT, "time_index")));      //creates the signal that sends the time index to other functions
    ADD_SIGNAL(MethodInfo("clock_tracker_signal",
        PropertyInfo(Variant::INT, "time_index"), 
        PropertyInfo(Variant::BOOL, "play_state"), 
        PropertyInfo(Variant::INT, "direction")));      //create the signal that is sent to the canvascontrol node so it can show the status of the simulation
}

Timebase::Timebase(){       //initialize the timer with some arbitrary standard values
    amount = 5;             //set the initial time it takes for the timer to finish to 5 seconds
    flowdirection = 1;      //set the initial direction of time flow to be positive
    timeindex = 0;          //set the initial time index to 0
    pause_state = false;    //set the initial pause state boolean to false because the timer starts as soon as the program launches
}

Timebase::~Timebase(){
}

void Timebase::_ready(){                                //this function is called after the constructor and is mainly used for further initialization purposes
    timebase->set_wait_time(amount);                                //set the timer wait time to the amount specified as a class variable
    timebase->set_autostart(true);                                  //automatically start the timer
    timebase->set_one_shot(false);                                  //turn off one shot so the timer runs an infinite number of times
    timebase->set_timer_process_callback(TIMER_PROCESS_IDLE);       //make the timer update after every rendered frame
    add_child(timebase);                                                                //add this new timer as a child to this node          
    timebase->connect("timeout", Callable(this, "_on_timebase_timeout"));               //internally connect the timeout signal of the timer to the _on_timebase_timeout function

    Node* parentnode = get_parent();                                        //find the parent node of this timebase node instance
    Node2D* arduinonode = parentnode->get_node<Node2D>("Timebase");         //using the parent node find the arduino controlling node
    if (arduinonode){
        arduinonode->connect("SMR", Callable(this, "_on_direction_signal"));        //connect the direction managing signal to the timebase node
    }
    else{
        UtilityFunctions::print("Arduino connection node not found");               //print an error when the arduino node can not be found
    }
}

void Timebase::_on_timebase_timeout(){                  //this function is called every time the timer sends its timeout signal
    UtilityFunctions::print("index: ", timeindex, " amt: ", amount, " dir: ", flowdirection);
    emit_signal("time_index_signal", timeindex);                                                //emit a signal that sends the current time index to other nodes
    emit_signal("clock_tracker_signal", timeindex, pause_state, flowdirection);                 //emit a signal that sends the current time index, pause state and time flow direction to other nodes
    timeindex = timeindex + flowdirection;                                                      //update the time index to the new value
}

int Timebase::_get_time_index(){                       //this function is used to return the private class variable timeindex to other classes
    UtilityFunctions::print("returning time index from timebase", timeindex);   
    return timeindex;
}

void Timebase::_on_direction_signal(String message){    //this function determines the direction in which time should flow or if it should pause
    int selector = message.to_int();
    switch(selector){                                   //case statement for signal handling
                case -1:
                    flowdirection = -1;                 //revert time
                    break;

                case 1:
                    flowdirection = 1;                  //forward time
                    break;

                case 2:
                    if (!timebase->is_paused()){
                        timebase->set_paused(true);     //pause
                        pause_state = true;
                    }
                    break;

                case 3:
                    if (timebase->is_paused()){
                        timebase->set_paused(false);    //unpause
                        pause_state = false;
                    }
                    break;

                default:
                    break;
                }
    return;
}

void Timebase::set_timer_amount(const float new_amount){       //This function updates the time which it takes the timer to end
    amount = new_amount;                                       //update the private class variable to keep track of the current value
    timebase->set_wait_time(amount);                           //update the new timer length in the timer itself
    return;
}

int Timebase::get_timer_amount(){                              //returns the current timer length
    return amount;
}

void Timebase::_input(InputEventKey *event){                    //input handling system for the keyboard, used mainly for debugging but can also implement other functionality
    if (timebase){
        Ref<InputEventKey> key_btn = event;
        if (key_btn->get_keycode() && key_btn->is_pressed()){
            int keyselector = key_btn->get_keycode();
            UtilityFunctions::print(keyselector);
            switch(keyselector){
                case 65:                                             //65 corresponds to the keyboard input "a" and speeds up the simulation
                    if(amount > 0){                             
                        old_amount = amount;
                        amount = amount - 0.25;
                        if ((old_amount - 0.25) <= 0){
                            UtilityFunctions::print("Can't go slower!");        //minimum time between updates is set to 0.25 seconds
                        }
                        else{
                            timebase->set_wait_time(amount);                    //if not minimum set new timer speed
                        }     
                    }
                    break;

                case 68:                                            //68 corresponds to the keyboard input "d" and slows down the simulation
                    amount = amount + 0.25;
                    timebase->set_wait_time(amount);                //set new timer time
                    break;

                default:
                    break;
                }
            }
        }
        return;
    }