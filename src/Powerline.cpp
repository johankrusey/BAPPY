#include "powerline.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

using namespace godot;

void Powerline::_bind_methods() {// The bind methods code is used to assign characteristics that can be edited inside of Godot, but we do not need any for the powerlines
    ADD_SIGNAL(MethodInfo("red_lines_signal", PropertyInfo(Variant::INT, "red_lines")));
    ADD_SIGNAL(MethodInfo("overloaded_lines_signal", PropertyInfo(Variant::INT, "overloaded_lines")));
}

Powerline::Powerline() {
}

Powerline::~Powerline() { // When this class is destructed it should still clean up the lines, otherwise you get leakage
    _cleanup_powerlines();
}

void Powerline::_cleanup_powerlines() {// Here we clean up dynamically allocated Line2D and Polygon2D nodes
    Array children = get_children(); // This finds all the children and places those in an array
    for (int i = 0; i < children.size(); i++) { // Iterate over the children, point pointers to them and then kill them
        Node* child = Object::cast_to<Node>(children[i]);
        if (child && (Object::cast_to<Line2D>(child) || Object::cast_to<Polygon2D>(child))) {
            remove_child(child);
            child->queue_free();
        }
    }
}

void Powerline::_create_powerlines(int arr[][4],int rowCount) { // This is a function that creates all the powerlines and the arrows on them (polygons)
    std::ifstream inputFile("C:\\Users\\rikie\\Desktop\\BAP\\Godot\\bap\\src\\Output_lines.csv"); // This is not relative pathing due to Godot not liking csv files in its path
    if (!inputFile) {
        UtilityFunctions::printerr("Error opening output_lines file!"); // Debugging message in case the pathing is messed up
        return;
    }

    std::vector<float> importantData; // This initializes the variable that stores the %Loading of each line, this comes from the load_flow.hpp file
    std::vector<int> directions;  // This initializes the variable that stores the direction of flow on each line, this comes from the load_flow.hpp file
    importantData.reserve(rowCount); // Allocate the space
    directions.reserve(rowCount); // Again you got to leave the space

    std::string line;
    if (!std::getline(inputFile, line)) { // This code reads the header line and does basically nothing with it
        UtilityFunctions::printerr("Error reading header line!");
        return;
    }

    while (std::getline(inputFile, line)) { // This code reads the inputs from the csv file and makes it so that we can work with its data
        std::istringstream iss(line);
        std::string busFrom, busTo, importantColumnStr, directionStr;

        if (!std::getline(iss, busFrom, ',') ||
            !std::getline(iss, busTo, ',') ||
            !std::getline(iss, importantColumnStr, ',') ||
            !std::getline(iss, directionStr, ',')) {
            UtilityFunctions::printerr("Error reading line: ", line.c_str());
            continue;
        }

        try {
            float importantColumn = std::stof(importantColumnStr);
            int direction = std::stoi(directionStr);
            importantData.push_back(importantColumn);
            directions.push_back(direction);
        }
        catch (const std::invalid_argument&) {
            UtilityFunctions::printerr("Invalid data format: ", importantColumnStr.c_str());
        }
    }

    inputFile.close(); // Closes the input file

    if (importantData.size() != rowCount) { // If you messed up either the amount of info in the csv file or the array in the mapgenerator.h you will see this message
        UtilityFunctions::printerr("Warning: Expected ", rowCount, " rows but read ", importantData.size(), " rows.");
    }

    int overloaded_lines = 0;
    int red_lines = 0;
    for (int k = 0; k < rowCount; ++k) { // Iterate over all the lines in arr[][]
        if (k >= importantData.size()) {
            UtilityFunctions::printerr("Error: Not enough data to iterate over ", rowCount, " rows.");
            break;
        }

        Vector2 startPoint(8 + 16 * arr[k][0], 8 + 16 * arr[k][1]); // This sets the begin and end points of the line based on the information in the arr[][] array
        Vector2 endPoint(8 + 16 * arr[k][2], 8 + 16 * arr[k][3]);

        Line2D* line = memnew(Line2D); // Here the lines are created
        line->add_point(startPoint); // Here the start point is assigned
        line->add_point(endPoint); // Here the end point is assigned
        line->set_width(4); // Here the line width is set

        float value = importantData[k]; // Here the value that is read from the csv file is cast from a string to a float
        Color lineColor; // For info see https://docs.godotengine.org/en/stable/classes/class_color.html#class-color
        if (value > 100) { // This code selects the line color based on the percentage loading
            lineColor = Color(0, 0, 0, 1); // Black for overloaded lines
            overloaded_lines += 1;
        }
        else if (value > 95) {
            red_lines += 1;
            lineColor = Color(1, 0, 0, 1); // Red for nearly overloaded lines 95+%
        }
        else if (value > 75) {
            lineColor = Color(1, 0.647059, 0, 1); // Orange for more than 75
        }
        else if (value > 50) {
            lineColor = Color(0.678431, 1, 0.184314, 1); // Green-Yellow for more than half (because yellow was not visible enough) 
        }
        else if (value < 1){
            lineColor = Color(0, 0.8, 0.831373, 1); // White smoke color for "unused" lines
        }
        else {
            lineColor = Color(0.196078, 0.803922, 0.196078, 1); // Lime-Green for less than half
        }
    
        line->set_default_color(lineColor); // Set the color
        add_child(line); // Add it as a child to the Node2D

        // This code will generate an arrow in the middle of the line pointing in the direction of flow in the same color as the line
        // Calculate midpoint and direction for arrow
        Vector2 midpoint = (startPoint + endPoint) / 2; // Calculates the place where it should be generated
        Vector2 direction = (endPoint - startPoint).normalized(); // It calculates the angle in which the line points

        if (directions[k] == -1) { // Possibly invert direction based on the direction value from the CSV
            direction = -direction;
        }

        Polygon2D* arrow = memnew(Polygon2D);  // Create the arrowhead as a Polygon2D type

        float arrowSize = 15;  // Define the arrowhead size
        PackedVector2Array arrowPoints; // These commands draw the triangle shape
        arrowPoints.push_back(Vector2(0, -arrowSize / 2));
        arrowPoints.push_back(Vector2(arrowSize, 0));
        arrowPoints.push_back(Vector2(0, arrowSize / 2));

        arrow->set_polygon(arrowPoints); // With all that information the polygon is set

        float angle = std::atan2(direction.y, direction.x); // Rotate the arrow to point in the correct direction
        arrow->set_rotation(angle);
        arrow->set_position(midpoint); // Set the arrow position to the midpoint
        arrow->set_modulate(lineColor); // Set the arrow color to match the line color

        add_child(arrow); // Add the arrow as a child of the Node2D
    }
    emit_signal("red_lines_signal", red_lines);
    emit_signal("overloaded_lines_signal", overloaded_lines);
}