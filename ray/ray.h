//created by alvaregd 10/12/15.

/***
 * Ray Object
 *
 * This object is just a translucent quad with a shader that draws light rays on it.
 * This object works best in underwater scenes, but it is possible to modify its behaviour
 * for other purposes such as light breaking in between the clouds.
 *
 * The original algorithm for generating light is found here:
 * https://www.shadertoy.com/view/lljGDt
 *
 * This program takes one instance of this object and renders it many times
 * in random locations and with different ages. The quads are given a billboard effect.
 */


#ifndef AQUARIUM_LIGHTRAYS_H
#define AQUARIUM_LIGHTRAYS_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>

#define RAY_VERTEX "./ray/ray.vert"
#define RAY_FRAGMENT "./ray/ray.frag"

#define RAY_LIFE_SPAN 3.14     //amount of time (in seconds) for ray to shine
#define RAY_COUNT 50           //number of rays quads to display
#define RAY_AREA_WIDTH 500.0f  //total width of the area in which quads will appear
#define RAY_AREA_HEIGHT 500.0f //total depth of the area in which quads will appear

struct Ray {

    GLint location_modelMatrix;
    GLint location_viewMatrix;
    GLint location_projectionMatrix;
    GLint location_resolution;
    GLint location_globalTime;       //controls the appearance of the light
    GLint location_life;             //controls visibilty of the quad
    GLint location_skyColour;        //controls fog colour
    GLint location_fogDensity;
    GLint location_fogGradient;

    GLuint shader;
    GLuint vao;
    GLuint texCoordinateVbo;
    GLuint positionsVbo;
    GLuint tex;

    GLfloat fogDensity = 0.007f;
    GLfloat fogGradient = 1.5f;

    mat4 modelMatrix;
    mat4 T;
    mat4 S;

    float rayHeight = -30.0f;   //height of the quad
    float* rayTimers;           //time displacement values (so that quads don't all animate exactly the same)
    float* rayX;                //x position of quad
    float* rayZ;                // z position of quad
};

/**
 * initialize the ray object. Call this function before calling rayRender
 * out ray: object to initialize
 * in proj_mat: projection matrix
 */
void rayInit(Ray *ray, GLfloat *proj_mat);

/** create vao and their vbos */
void rayCreateVao(Ray *ray);

/** get shader locations
 * out ray: object containing the locations **/
void rayGetUniforms(Ray * ray);

/**
 *render ray object
 * in ray: ray object to render
 * in camera:
 * in isAbovewater: indicates if camera is above or below water
 * in globalTime: curren time in milliseconds
 * in elapsedTime: time since start of program
 */
void rayRender(Ray *ray, Camera *camera, bool isAboveWater, double globalTime, double elapsedTime) ;

/**
 * update ray object, in this case apperance of light
 * in ray: object to update
 * in index: indicate which copy of this object should be updated
 * in elapsedSeconds: time since program started
 */
void rayUpdate(Ray * ray ,int index, double elapsedSeconds);

#endif //AQUARIUM_LIGHTRAYS_H