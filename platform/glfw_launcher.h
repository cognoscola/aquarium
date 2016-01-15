//
// Created by alvaregd on 27/11/15.
//

#ifndef SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H
#define SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <utils/log_utils/logger.h>

struct Window {
    GLFWwindow* window;
    GLFWmonitor *wmonitor;
    const GLFWvidmode* vmode;
};

/**
 * Start an opengl instance
 * in window: window object to start the instance
 * returns true if execution finished successfully
 */
bool start_gl(Window *window);

/**
 * function to call when an error happens
 */
void glfw_error_callback(int error, const char* description);




#endif //SIMPLE_LEVEL_EDITOR_GLFW_LAUNCHER_H
