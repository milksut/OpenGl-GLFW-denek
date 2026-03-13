//
// Created by altay2510tr on 3/13/26.
//
#pragma once

#include "Globals.h"
#include "The_event_manager.h"
#include "Events/Keyborad_events.h"
#include "Events/Mouse_events.h"

///Works on 1 window, crate 1 manager per window, and use it on same thread.
///You can set down streams if you want to transport event but
///events only sent to next only if they are not consumed!
///Run Poll_keys every tick or so to check key event, it's not work by himself
class Input_Manager
{
    private:
    void mouse_callback(double x_pos, double y_pos)
    {
        const double mouse_x_offset= (x_pos - mouse_x) * mouse_sensitivity;
        const double mouse_y_offset = (y_pos - mouse_y) * mouse_sensitivity;

        mouse_x = x_pos;
        mouse_y = y_pos;

        event_manager.throw_event("Mouse_input", std::make_unique<Mouse_move_event>(
            mouse_x_offset, mouse_y_offset));
    }

    public:
    bool pressed[GLFW_KEY_LAST] = {false};

    double mouse_x=0, mouse_y=0, mouse_sensitivity = 1;

    Event_manager& event_manager;
    GLFWwindow* window;

    ///width and height only used for starting mouse positon
    Input_Manager(Event_manager& event_manager, GLFWwindow* window,const int window_width = 0,const int window_height = 0)
        : event_manager(event_manager), window(window)
    {
        event_manager.create_channel("Keyboard_input");
        event_manager.create_channel("Mouse_input");

        mouse_x = window_width / 2.0;
        mouse_y = window_height / 2.0;

        //TODO: when Window class is ready, use window class as user pointer and use input manger from there
        glfwSetWindowUserPointer(window, this);
        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x_pos, double y_pos)
        {
            const auto self = static_cast<Input_Manager*>(glfwGetWindowUserPointer(window));
            self->mouse_callback(x_pos, y_pos);
        });
    }

    void Poll_keys()
    {
        for(int key = 0; key <= GLFW_KEY_LAST; key++)
        {
            bool is_pressed = glfwGetKey(window, key) == GLFW_PRESS;
            bool was_pressed = pressed[key];

            if(is_pressed && was_pressed)
            {
                event_manager.throw_event("Keyboard_input", std::make_unique<Key_hold_event>(key));
            }
            else if(is_pressed)
            {
                event_manager.throw_event("Keyboard_input", std::make_unique<Key_press_event>(key));
            }
            else if(was_pressed)
            {
                event_manager.throw_event("Keyboard_input", std::make_unique<Key_release_event>(key));
            }

            pressed[key] = is_pressed;
        }
    }

};