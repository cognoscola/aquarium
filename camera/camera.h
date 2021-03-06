//
// Created by alvaregd on 02/12/15.
//
/**
 * Camera
 *
 * An object that stores movement information about the camera.
 * This can include the camera's angle, velocity, position etc.
 */


#ifndef WATER_REFLECTION_CAMERA_H
#define WATER_REFLECTION_CAMERA_H

#include <input/input.h>
#include "utils/math_utils/maths_funcs.h"
#include "GL/gl.h"

struct Window;

/**
 * Holds information about the camera such as
 * its position, orientation and projection
 */
struct Camera{
    float pos[3]; // don't start at zero, or we will be too close
    float yaw = 0.0f; // y-rotation in degrees
    float pitch = 0.0f;
    float roll = 0.0f;
    float signal_amplifier = 0.1f;

    mat4 T;
    mat4 Rpitch;
    mat4 Ryaw;
    mat4 viewMatrix;


    int pushing; //-1 slowing down, +1 accelerating , 0 = idle
    bool moving; //velocity != 0
    double move_angle;

    vec3 velocity; //actor's velocity

    GLfloat speedConstant = 0.5f;
    GLfloat* proj_mat;
};

/** initialize the camera. For now its state is hardcoded
 *  in - Camera object
 *  in - Window object
 **/
void cameraInit(Camera * camera, Window *window);

/**
 * calculate the camera's postion
 * in camera: the main camera
 * in input: keyboard input
 */
void updateMovement(Camera* camera, Input* input);


void calculateViewMatrices(Camera *camera);
#endif //WATER_REFLECTION_CAMERA_H
