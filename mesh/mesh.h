//
// Created by alvaregd on 06/12/15.
//
#ifndef WATER_REFLECTION_MESH_H
#define WATER_REFLECTION_MESH_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>
#include <camera/camera.h>

#define MAP_TEXTURE "/home/alvaregd/Documents/Games/aquarium/assets/ambient_oclusion.png"
#define MAP_FILE "/home/alvaregd/Documents/Games/aquarium/assets/map.obj"

#define MESH_VERTEX "/home/alvaregd/Documents/Games/aquarium/mesh/mesh.vert"
#define MESH_FRAGMENT "/home/alvaregd/Documents/Games/aquarium/mesh/mesh.frag"

#define RED_CORAL "/home/alvaregd/Documents/Games/aquarium/assets/red_coral.obj"

struct Mesh{

    int numCopies;

    GLuint shader;
    GLuint texture;
    GLuint vao;
    GLuint vbo;

    GLint location_projection_mat;
    GLint location_view_mat;
    GLint location_model_mat;
    GLint location_clip_plane;

    GLint location_baseTexture;
    GLint location_luminanceTexture;
    GLint location_skyColour;
    GLint location_fogDensity;
    GLint location_fogGradient;

    GLfloat fogDensity = 0.007f;
    GLfloat fogGradient = 1.5f;

    mat4* modelMatrix;
    int vertexCount;

    GLuint* causticTextureIds;
    int causticIndex = 0;
    double MAX_COUSTIC_TIME = 0.05f;
    double timer;
};

struct MeshObject{

    char objfilename[100];
    char* texFilenames;

    int index = 0;
    int numberOfCopies;
    mat4* T;
    mat4* S;
    mat4* R;

};

struct MeshCollection{

    int numberOfFiles;
    int fileIndex = -1;
    MeshObject* meshObject;

    Mesh* mesh;

    bool hasR;
    bool hasT;
    bool hasS;
};


void meshInit(Mesh* mesh, char* filename, char* texFilename ,GLfloat* proj_mat);
bool meshLoadMeshFile(const char *fileName, GLuint *vao, int *point_count);
void meshSetInitialTransformation(Mesh* mesh, mat4* T, mat4* S,mat4* R );
void meshLoadTexture(Mesh* mesh, char* filename);
void meshLoadShaderProgram(Mesh * mesh);
void meshGetUniforms(Mesh* mesh);
void meshRender(Mesh* mesh, Camera* camera, GLfloat planeHeight, bool isAboveWater);
void meshCleanUp(Mesh *mesh);
void meshLoadCausticTexture(Mesh* mesh);
void meshUpdate(Mesh *mesh, double elapsed_seconds);
#endif //WATER_REFLECTION_MESH_H
