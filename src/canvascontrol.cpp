#include "canvascontrol.h"
#include "popupbox.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>


using namespace godot;

void CanvasControl::_bind_methods() { 
    ClassDB::bind_method(D_METHOD("check_child_ready"), &CanvasControl::check_child_ready);
    ClassDB::bind_method(D_METHOD("_update_clock_labels"), &CanvasControl::_update_clock_labels);
}

CanvasControl::CanvasControl(){
    set_position(Vector2(500,0));            //camera node is shifted 500 points on the positive x axis so this should also happen for the control node
    set_size(Vector2(2560,1500));                //camera zoom is 0.4, with the standard size being 1024x600. So the control node should be 2560x1500
    set_focus_mode(Control::FocusMode::FOCUS_NONE);
    flatbox = memnew(StyleBoxFlat);
    flatbox->set_border_color(Color(0.827451, 0.827451, 0.827451, 1));
    flatbox->set_border_width_all(2);
    flatbox->set_bg_color(Color(0.972549, 0.972549, 1, 1));
    flatbox->set_content_margin_all(0);
    boxtheme = memnew(Theme);
    boxtheme->add_type("Control");
    boxtheme->add_type("Label");
    boxtheme->add_type("VBoxContainer");
    boxtheme->add_type("HBoxContainer");
    boxtheme->add_type("MarginContainer");
    boxtheme->set_constant("separation", "VBoxContainer",50);
    boxtheme->set_constant("separation", "HBoxContainer", 15);
    boxtheme->set_constant("margin_left", "MarginContainer", 15);
    boxtheme->set_constant("margin_right", "MarginContainer", 15);
    boxtheme->set_constant("margin_top", "MarginContainer", 10);
    boxtheme->set_constant("margin_bottom", "MarginContainer", 10);
    boxtheme->set_stylebox("panel", "Control", flatbox);
    boxtheme->set_color("font_color","Label", Color(0,0,0,1));
    set_theme(boxtheme);
    set_mouse_filter(Control::MouseFilter::MOUSE_FILTER_IGNORE);
}

CanvasControl::~CanvasControl(){
/*     if (flatbox) {
        memdelete(flatbox); // Delete stylebox from memory on 
    }
    if (boxtheme) {
        memdelete(boxtheme); //
    } */
}

void CanvasControl::_ready(){
    UtilityFunctions::print("canvascontrol ready");
    PopupBox* popup_line_legend = memnew(PopupBox);
    popup_line_legend->_create_line_legend();
    PopupBox* popup_icon_legend = memnew(PopupBox);
    popup_icon_legend->_create_icon_legend();
    _create_clock_labels();
    add_child(popup_line_legend);
    add_child(popup_icon_legend);

    Node* rootnode = get_parent();
    if (rootnode) {
        Timebase* timenode = rootnode->get_node<Timebase>("Timebase");
        if (timenode) {
            timenode->connect("clock_tracker_signal", Callable(this, "_update_clock_labels"));
        }
        else{
            UtilityFunctions::print("error getting timebase");
        }
    }
    UtilityFunctions::print("line legend: ", popup_line_legend);
    UtilityFunctions::print("icon legend: ", popup_icon_legend);
}

void CanvasControl::_enter_tree(){
    UtilityFunctions::print("canvascontrol entered tree");
}

void CanvasControl::_delete_popup(){
    UtilityFunctions::print("canvascontrol delete popup called");
    get_child(4)->queue_free();
    UtilityFunctions::print("canvascontrol delete popup finished");
    return;
}

void CanvasControl::check_child_ready(int xpos, int ypos){
    Node* child = get_child(4);
    PopupBox* popup = get_node<PopupBox>(child->get_path());
    if (popup){
        popup->_create_content(xpos, ypos);
    }
    else{
        UtilityFunctions::print("child not found");
    }
    return;
}

void CanvasControl::_create_popup(int xpos, int ypos){
    UtilityFunctions::print("canvascontrol create popup called");
    popup = memnew(PopupBox);
    call_deferred("check_child_ready", xpos, ypos);
    add_child(popup);
    UtilityFunctions::print("canvascontrol create popup finished");
    return;
}

void CanvasControl::_create_clock_labels(){
    Label* clocklabel = memnew(Label);
    clocklabel->set_anchors_preset(Control::LayoutPreset::PRESET_TOP_RIGHT, false);
    clocklabel->set_offset(Side::SIDE_TOP, 25);
    clocklabel->set_offset(Side::SIDE_BOTTOM, 100);
    clocklabel->set_offset(Side::SIDE_RIGHT, -40);
    clocklabel->set_offset(Side::SIDE_LEFT, -500);
    clocklabel->set_horizontal_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_RIGHT);
    clocklabel->set_text("00:00");
    clocklabel->add_theme_font_size_override("font_size", 75);
    add_child(clocklabel);
    Label* statuslabel = memnew(Label);
    statuslabel->set_anchors_preset(Control::LayoutPreset::PRESET_TOP_RIGHT, false);
    statuslabel->set_offset(Side::SIDE_TOP, 125);
    statuslabel->set_offset(Side::SIDE_BOTTOM, 200);
    statuslabel->set_offset(Side::SIDE_RIGHT, -40);
    statuslabel->set_offset(Side::SIDE_LEFT, -500);
    statuslabel->set_horizontal_alignment(HorizontalAlignment::HORIZONTAL_ALIGNMENT_RIGHT);
    statuslabel->set_text("paused");
    statuslabel->add_theme_font_size_override("font_size", 75);
    add_child(statuslabel);
}

void CanvasControl::_update_clock_labels(int time_index, bool pause_state, int direction){
    Node* clocknode = get_child(0);
    Node* statusnode = get_child(1);
    Label* clocklabel = get_node<Label>(clocknode->get_path());
    Label* statuslabel = get_node<Label>(statusnode->get_path());
    std::string time;
    if (time_index < 10){
        time = "0" + std::to_string(time_index) + ":00";
    }
    else{
        time = std::to_string(time_index) + ":00";
    }
    clocklabel->set_text(time.c_str());
    if (pause_state){
        statuslabel->set_text("Paused");
    }
    else{
        UtilityFunctions::print(direction);
        if (direction == -1){
            statuslabel->set_text("Reverting");
        }
        else{
            statuslabel->set_text("Simulating");
        }
    }
}