#ifndef POPUPBOX_H
#define POPUPBOX_H

#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/popup_panel.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/color_rect.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/texture2d.hpp>


namespace godot {
    class PopupBox : public PopupPanel {
        GDCLASS(PopupBox, PopupPanel)

    private:
        Panel* panel;
        Label* label;
        VBoxContainer* textcontainer;
        int tile_x;
        int tile_y;
        int initial_index;
        int previous_index;
        int bus_index;
    protected:
        static void _bind_methods();

    public:
        PopupBox();
        ~PopupBox();

        void _ready();
        void _get_node_index();
        int* _get_node_data(int node_index, int time_index);
        void _create_label(int* labeldata, String labeltext);
        void _update_label(int* labeldata, String labeltext, int time_index, int labelnr);
        void _create_content(int xloc, int yloc);
        void _popup_update(int time_index);
        void _create_line_legend();
        void _create_line_legend_entry(String text, Color color);
        void _create_icon_legend();
        void _create_icon_legend_entry(String text, String resource_path);
    };

} // namespace godot

#endif // POPUPBOX_H
