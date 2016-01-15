//
// Created by alvaregd on 27/11/15.
//

#include <GL/glew.h>
#include "glfw_launcher.h"

bool start_gl (Window *window) {

    const GLubyte* renderer;
    const GLubyte* version;

    gl_log ("starting GLFW %s", glfwGetVersionString ());

    glfwSetErrorCallback (glfw_error_callback);
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    //make the window full screen
    window->wmonitor = glfwGetPrimaryMonitor();
    window->vmode = glfwGetVideoMode(window->wmonitor);
    window->window = glfwCreateWindow (window->vmode->width, window->vmode->height, "Hello World", window->wmonitor, NULL);
    if (!window->window) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent (window->window);

    glewExperimental = GL_TRUE;
    glewInit ();

    renderer = glGetString (GL_RENDERER); /* get renderer string */
    version = glGetString (GL_VERSION); /* version as a string */
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    return true;
}

void glfw_error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

