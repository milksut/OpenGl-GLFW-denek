//
// Created by altay2510tr on 3/13/26.
//
#pragma once

#include "Globals.h"

class Key_press_event: public Event_management::Event
{
    public:
    int key_code;
    Key_press_event(const int key_code)
        : Event(Event_management::Event_timing::Immediate, Event_management::Event_type::Key_pressed), key_code(key_code)
    {}

    void execute() override {}
};

class Key_hold_event: public Event_management::Event
{
public:
    int key_code;
    Key_hold_event(const int key_code)
        : Event(Event_management::Event_timing::Immediate, Event_management::Event_type::Key_hold), key_code(key_code)
    {}

    void execute() override {}
};

class Key_release_event: public Event_management::Event
{
public:
    int key_code;
    Key_release_event(const int key_code)
        : Event(Event_management::Event_timing::Immediate, Event_management::Event_type::Key_released), key_code(key_code)
    {}

    void execute() override {}
};