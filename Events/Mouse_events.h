//
// Created by altay2510tr on 3/13/26.
//
#pragma once
#include "Globals.h"

class Mouse_move_event: public Event_management::Event
{
public:
    double mouse_x_offset, mouse_y_offset;
    Mouse_move_event(const double mouse_x_offset, const double mouse_y_offset)
        : Event(Event_management::Event_timing::Immediate, Event_management::Event_type::Mouse_moved),
        mouse_x_offset(mouse_x_offset), mouse_y_offset(mouse_y_offset)
    {}

    void execute() override {}
};