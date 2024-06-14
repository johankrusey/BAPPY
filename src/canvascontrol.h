#ifndef CANVASCONTROL_H
#define CANVASCONTROL_H

#include "timebase.h"
#include "popupbox.h"
#include <godot_cpp/classes/popup.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/theme.hpp>


namespace godot {

    class CanvasControl : public Control {
        GDCLASS(CanvasControl, Control)

    private:
        StyleBoxFlat* flatbox;
        Theme* boxtheme;
        PopupBox* popup;
    protected:
        static void _bind_methods();

    public:
        CanvasControl();
        ~CanvasControl();

        void _ready();
        void _enter_tree();
        void _create_popup(int xpos, int ypos);
        void _delete_popup();
        void _create_legend();
        void check_child_ready(int xpos, int ypos);
        void _create_clock_labels();
        void _update_clock_labels(int time_index, bool pause_state, int direction);
    };

} // namespace godot

#endif // CANVASCONTROL_H
