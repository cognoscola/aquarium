#include <GL/glew.h>
#include <platform/glfw_launcher.h>
#include <utils/io/shader_loader.h>
#include <camera/camera.h>
#include <utils/io/texture.h>
#include "ray.h"

void rayInit(Ray *ray, GLfloat *proj_mat) {

    rayLoadTexture(ray, RAY_TEX);
    rayCreateVao(ray);
    ray->shader = create_programme_from_files(RAY_VERTEX, RAY_FRAGMENT);

    glUseProgram(ray->shader);
    rayGetUniforms(ray);

    glUniformMatrix4fv(ray->location_projectionMatrix, 1, GL_FALSE, proj_mat);
    glUniform1i(ray->location_baseTexture,0);
    ray->modelMatrix = scale(identity_mat4(), vec3(5.0f, 5.0f,5.0f));
    glUniformMatrix4fv(ray->location_modelMatrix, 1, GL_FALSE, ray->modelMatrix.m);

}

void rayCreateVao(Ray *ray){

    GLfloat texcoords[] = {
            1.0f,1.0f,
            0.0f,1.0f,
            0.0f,0.0f,
            0.0f,0.0f,
            1.0f, 0.0f,
            1.0f,1.0f,
    };

    GLfloat world_coordinates[] = {
            -0.75f, 0.25f,  0.0f,
            -0.25f, 0.25f,  0.0f,
            -0.25f, 0.75f,  0.0f,
            -0.25f, 0.75f,  0.0f,
            -0.75f, 0.75f,  0.0f,
            -0.75f, 0.25f,  0.0f
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), world_coordinates, GL_STATIC_DRAW);
    ray->positionsVbo = vbo;

    GLuint texCoordVbo;
    glGenBuffers(1, &texCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
    ray->texCoordinateVbo = texCoordVbo;

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, ray->positionsVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, ray->texCoordinateVbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    ray->vao = vao;
}

void rayGetUniforms(Ray * ray) {
    ray->location_baseTexture       = glGetUniformLocation(ray->shader, "baseMap");
    ray->location_modelMatrix       = glGetUniformLocation(ray->shader, "modelMatrix");
    ray->location_viewMatrix        = glGetUniformLocation(ray->shader, "viewMatrix");
    ray->location_projectionMatrix  = glGetUniformLocation(ray->shader, "projectionMatrix");
}


void rayRender(Ray* ray, Camera* camera, bool isAboveWater){


    glUseProgram(ray->shader);
    glUniformMatrix4fv(ray->location_viewMatrix, 1, GL_FALSE, camera->viewMatrix.m);
    glBindVertexArray(ray->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ray->tex);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

void rayLoadTexture(Ray* ray, const char* name){

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    unsigned char* image_data;
    int x ,y;
    loadImageFile(name, true, &image_data, &x,&y);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    free(image_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    ray->tex = texID;
}
