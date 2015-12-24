#ifndef AQUARIUM_LIGHTRAYS_H
#define AQUARIUM_LIGHTRAYS_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>

#define RAY_VERTEX "/home/alvaregd/Documents/Games/aquarium/ray/ray.vert"
#define RAY_FRAGMENT "/home/alvaregd/Documents/Games/aquarium/ray/ray.frag"

#define RAY_TEX "/home/alvaregd/Documents/Games/aquarium/assets/back.png"

struct Ray {

    GLint location_baseTexture;
    GLint location_modelMatrix;
    GLint location_viewMatrix;
    GLint location_projectionMatrix;
    GLint location_resolution;
    GLint location_globalTime;

    GLuint shader;
    GLuint vao;
    GLuint texCoordinateVbo;
    GLuint positionsVbo;
    GLuint tex;

    mat4 modelMatrix;
    mat4 T;
    mat4 S;
};

void rayInit(Ray *ray, GLfloat *proj_mat);
void rayCreateVao(Ray *ray);
void rayGetUniforms(Ray * ray);
void rayLoadTexture(Ray* ray, const char* name);
void rayRender(Ray *ray, Camera *camera, bool isAboveWater, GLfloat globalTime);
#endif //AQUARIUM_LIGHTRAYS_H
