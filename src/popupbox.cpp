#include "popupbox.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/label_settings.hpp>
#include "mapgenerator.h"
#include <vector>
#include <complex>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstring>

using namespace godot;

void PopupBox::_bind_methods() { 
    ClassDB::bind_method(D_METHOD("_popup_update"), &PopupBox::_popup_update);
    ClassDB::bind_method(D_METHOD("_get_node_index"), &PopupBox::_get_node_index);
}

PopupBox::PopupBox(){
    set_exclusive(false);
    set_flag(Window::Flags::FLAG_NO_FOCUS, true);
    set_flag(Window::Flags::FLAG_BORDERLESS, true);
    set_transient(false);
    set_flag(Window::Flags::FLAG_ALWAYS_ON_TOP, true);
}

PopupBox::~PopupBox(){

}

void PopupBox::_ready(){
    Node* popupparent = get_parent();
    if (popupparent) {
        Node* rootnode = popupparent->get_parent();
        if (rootnode) {
            Timebase* timenode = rootnode->get_node<Timebase>("Timebase");
            if (timenode) {
                timenode->connect("time_index_signal", Callable(this, "_popup_update"));
                initial_index = timenode->_get_time_index();
                UtilityFunctions::print("Initial time index: ", timenode->_get_time_index());
            }
            else{
                UtilityFunctions::print("error getting timebase");
            }
        }
    }
    bus_index = 0;
}

void PopupBox::_create_content(int xloc, int yloc){
    tile_x = xloc;
    tile_y = yloc;
    set_position(Vector2(16*xloc*0.4 - 200 + 31*0.4,16*yloc*0.4 - 281*0.4)); //due to camera scaling and camera shift location should be *0.4 and -200 for x. 2.5 normal coords becomes 1
    show();
    panel = memnew(Panel);
    add_child(panel);
    MarginContainer* mgc = memnew(MarginContainer);
    add_child(mgc);
    VBoxContainer* textvbox = memnew(VBoxContainer);
    mgc->add_child(textvbox);
    _get_node_index();
    int* node_data = _get_node_data(bus_index, initial_index);
    std::cout << node_data << *node_data;
    _create_label(node_data, "Current load:");
    for (int l = 0; l < 5; l++){
        std::string temp1 = "t+" + std::to_string(l * 5 + 1) + "  ";
        _create_label((node_data + 1 + (l * 5)), temp1.c_str());
    }
    Vector2 loca = get_position();
    if (loca[1] < 0){
        set_position(Vector2(loca[0], (115 + loca[1]) + loca[1]));
    }
    else if (loca[1] > 450){
        set_position(Vector2(loca[0], loca[1] - (115 - (500 - loca[1]))));
    }
    return;
}

void PopupBox::_get_node_index(){
    Node* parentnode = get_parent();
    if (parentnode){
        Node* rootnode = parentnode->get_parent();
        if (rootnode) {
            Node* mapgennode = rootnode->get_child(2);
            if (mapgennode) {
                NodePath tilesetpath = mapgennode->get_child(0)->get_path();    //Hier gaat het fout, bus index call heeft locatie nodig maar die bestaat niet bij legend
                UtilityFunctions::print(tilesetpath);
                Tileset* tilenode = get_node<Tileset>(tilesetpath);
                UtilityFunctions::print(tilenode);
                if (tilenode){
                    bus_index = tilenode->_fetch_node_index(tile_x, tile_y); 
                    UtilityFunctions::print("bus index updated to ", bus_index);
                }
                else{
                    UtilityFunctions::print("no tilenode found");
                }
            }
            else{
                std::cerr << "could not find mapgenerator node" << "\n";
            }
        }
        else{
            std::cerr << "could not find root node" << "\n";
        }
    }
    else{
        std::cerr << "could not return node index" << "\n";
    }
    return;
}

int* PopupBox::_get_node_data(int node_index, int time_index){
    UtilityFunctions::print("       starting get_node_data");
    std::string line, word;
    int* node_data = new int[24];
    for (int k = 0; k < 25; k++){
        node_data[k] = 0;
    }
    std::fstream fin;
    fin.open("C:\\Users\\rikie\\Desktop\\BAP\\Godot\\bap\\src\\Output.csv", std::ios::in);
    if (!fin.is_open()) {
        std::cerr << "Could not find current bus data\n";
    }
    std::getline(fin, line, '\n');                  //remove header row
    for (int i = 0; i < node_index; i++){
        std::getline(fin, line, '\n');             //remove data from nodes that are not needed
    }
    std::getline(fin, line, '\n');                     //read the row that has the data of the wanted bus
    std::stringstream s(line);
    for (int k = 0; k < 11; k++){
        std::getline(s, word, ',');
        if (k == 3){
            float tempdata = stof(word);                //read only the power in MW
            tempdata *= 100;
            node_data[0] = tempdata;
        }                                     
    }            
    
    fin.close();
    fin.open("C:\\Users\\rikie\\Desktop\\BAP\\Godot\\bap\\src\\Forecast.csv", std::ios::in);
    if (!fin.is_open()) {
        std::cerr << "Could not find forecast data\n";
    }
    std::getline(fin, line, '\n');
    int skipindex = time_index * 28;
    UtilityFunctions::print("       updating forecasts for time_index ", time_index);
    for (int j = 0; j < skipindex; j++){
        getline(fin, line, '\n');               //remove data from nodes that are not the correct time step
    }
    bool node_found = false;
    for (int w = 0; w < 28; w++){
        std::getline(fin, line, '\n');
        std::stringstream z(line);
        std::getline(z, word, ',');                     //get bus index from forecast file
        if (stoi(word) == bus_index){
            node_found = true;
            for (int k = 0; k < 24; k++){
                std::getline(z, word, ',');
                node_data[k+1] = stoi(word);
            }
        }
    }
    fin.close();
    UtilityFunctions::print("       finished getting node data");
    return node_data;
}

void PopupBox::_popup_update(int time_index){
    UtilityFunctions::print("updating popup: ", this->get_name());
    UtilityFunctions::print("   updating to time index ", time_index, " with bus index: ", bus_index);
    if (time_index != previous_index && bus_index != 0){
        previous_index = time_index;
        int* node_data = _get_node_data(bus_index, time_index);
        _update_label(node_data, "Current load:", time_index, 0);
        for (int l = 0; l < 5; l++){
            std::string temp1 = "t+" + std::to_string(l * 5 + 1) + "  ";
            _update_label((node_data + 1 + (l * 5)), temp1.c_str(), time_index, l+1);
        }
    }
    UtilityFunctions::print("   updating popup done");
    return;
}

void PopupBox::_update_label(int* labeldata, String labeltext, int time_index, int labelnr){
    Node* parent = get_parent();
    if (parent){
        if (parent->get_child_count() == 5){
            Node* popup = parent->get_child(4);
            if (popup){
                Node* marginnode = popup->get_child(1);
                if (marginnode){
                    Node* vbox = marginnode->get_child(0);
                    if (vbox){
                            NodePath labelpath = vbox->get_child(labelnr)->get_path();
                            Label* label = get_node<Label>(labelpath);
                            std::string temp1 = labeltext.utf8().get_data() + std::to_string(*labeldata) + " MW";
                            label->set_text(temp1.c_str());
                    }
                    else{
                        std::cerr << "could not find vbox to update" << "\n";
                    }
                }
                else{
                    std::cerr << "could not find marginbox to update" << "\n";
                }
            }
            else{
                std::cerr << "could not find popup to update" << "\n";
            }
        }
        else{
            std::cerr << "No data popup exists" << "\n";
        }
    }
    else{
            std::cerr << "could not find parent of this popup" << "\n";
        }
    return;
}

void PopupBox::_create_label(int* labeldata, String labeltext){
    Node* marginnode = get_parent()->get_child(4)->get_child(1);
    if (marginnode) {
        Node* vboxnode = marginnode->get_child(0);
        if (vboxnode){
            label = memnew(Label);
            std::string temp1 = labeltext.utf8().get_data() + std::to_string(*labeldata) + " MW";
            label->set_text(temp1.c_str());
            label->set_custom_minimum_size(Vector2(0,16));
            label->set_h_size_flags(Control::SizeFlags::SIZE_EXPAND_FILL);
            label->set_v_size_flags(Control::SizeFlags::SIZE_EXPAND_FILL);
            vboxnode->add_child(label);
        }
        else{
            std::cerr << "could not find vbox to create new label in" << "\n";
        }
    }
    else{
            std::cerr << "could not find marginnode for this popup" << "\n";
    }
    return;
}

void PopupBox::_create_line_legend(){
    set_position(Vector2(20,20));
    show();
    panel = memnew(Panel);
    add_child(panel);
    MarginContainer* mgc = memnew(MarginContainer);
    add_child(mgc);
    VBoxContainer* textcontainer = memnew(VBoxContainer);
    mgc->add_child(textcontainer);
    Label* label1 = memnew(Label);
    textcontainer->add_child(label1);
    label1->set_text("Powerline load %");
    _create_line_legend_entry(">100%", Color(0, 0, 0, 1));
    _create_line_legend_entry("95-100%", Color(1, 0, 0, 1));
    _create_line_legend_entry("75-95%", Color(1, 0.647059, 0, 1));
    _create_line_legend_entry("50-75%", Color(0.678431, 1, 0.184314, 1));
    _create_line_legend_entry("1-50%", Color(0.678431, 1, 0.184314, 1));
    _create_line_legend_entry("0-1%", Color(0, 0.8, 0.831373, 1));
    return;
}

void PopupBox::_create_line_legend_entry(String text, Color color){
    Node* mgc = get_child(1);
    Node* textcontainer = mgc->get_child(0);
    HBoxContainer* hbox = memnew(HBoxContainer);
    ColorRect* color_rect = memnew(ColorRect);
    color_rect->set_color(color);
    color_rect->set_size(Vector2(16,16));
    color_rect->set_h_size_flags(Control::SizeFlags::SIZE_EXPAND_FILL);
    Label* linelabel = memnew(Label);
    linelabel->set_h_size_flags(Control::SizeFlags::SIZE_EXPAND_FILL);
    linelabel->set_text(text);
    hbox->add_child(color_rect);
    hbox->add_child(linelabel);
    textcontainer->add_child(hbox);
    return;
}

void PopupBox::_create_icon_legend(){
    set_position(Vector2(20,255));
    show();
    panel = memnew(Panel);
    add_child(panel);
    MarginContainer* mgc = memnew(MarginContainer);
    add_child(mgc);
    VBoxContainer* textcontainer = memnew(VBoxContainer);
    mgc->add_child(textcontainer);
    _create_icon_legend_entry("City", "res://assets/icon_city.png");
    _create_icon_legend_entry("Industry", "res://assets/icon_industry.png");
    _create_icon_legend_entry("Generator", "res://assets/icon_fossil.png");
    _create_icon_legend_entry("Nuclear", "res://assets/icon_nuclear.png");
    _create_icon_legend_entry("Solar", "res://assets/icon_solar.png");
    _create_icon_legend_entry("Wind", "res://assets/icon_windmill.png");
    _create_icon_legend_entry("Slack", "res://assets/icon_slack.png");
    return;
}

void PopupBox::_create_icon_legend_entry(String text, String resource_path){
    Node* mgc = get_child(1);
    Node* textcontainer = mgc->get_child(0);
    HBoxContainer* hbox = memnew(HBoxContainer);
    Ref<Texture> texture = ResourceLoader::get_singleton()->load(resource_path);
    TextureRect* texture_rect = memnew(TextureRect);
    texture_rect->set_texture(texture);
    texture_rect->set_size(Vector2(23,23));
    texture_rect->set_stretch_mode(TextureRect::StretchMode::STRETCH_KEEP_ASPECT);
    Label* iconlabel = memnew(Label);
    iconlabel->set_text(text);
    iconlabel->set_h_size_flags(Control::SizeFlags::SIZE_EXPAND_FILL);
    hbox->add_child(texture_rect);
    hbox->add_child(iconlabel);
    textcontainer->add_child(hbox);
    return;
}