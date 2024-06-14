#ifndef TIMEBASE_H
#define TIMEBASE_H

#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class Timebase : public Timer {
    GDCLASS(Timebase, Timer)        //timebase is a subclass of the timer type

private:
    Timer* timebase = new Timer;
    bool pause_state;
    int flowdirection;
    int timeindex;
    float amount;
    float old_amount;
    String message; 

protected:
    static void _bind_methods();
public:
    Timebase();
    ~Timebase();

    void _ready();
    void _on_timebase_timeout();
    void _on_direction_signal(String message);
    int _get_time_index();

    int get_timer_amount();
    void set_timer_amount(float new_amount);
    void _input(InputEventKey *event);

};
}

#endif
