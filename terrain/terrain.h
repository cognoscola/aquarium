//
// Created by alvaregd on 26/12/15.
//

#ifndef AQUARIUM_TERRAIN_H
#define AQUARIUM_TERRAIN_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>
#include <camera/camera.h>

#define MAP_TEXTURE "/home/alvaregd/Documents/Games/aquarium/assets/ambient_oclusion.png"
#define MAP_FILE "/home/alvaregd/Documents/Games/aquarium/assets/map.obj"
#define FLOOR_FILE "/home/alvaregd/Documents/Games/aquarium/assets/floor1.obj"
#define FLOOR_TEXTURE "/home/alvaregd/Documents/Games/aquarium/assets/terrain_texture.png"

#define MESH_VERTEX "/home/alvaregd/Documents/Games/aquarium/mesh/mesh.vert"
#define MESH_FRAGMENT "/home/alvaregd/Documents/Games/aquarium/mesh/mesh.frag"

#define MESH_TERRAIN_UNDER_VERTEX "/home/alvaregd/Documents/Games/aquarium/mesh/terrain_underwater.vert"
#define MESH_TERRAIN_UNDER_FRAG "/home/alvaregd/Documents/Games/aquarium/mesh/terrain_underwater.frag"

#define MESH_TERRAIN 0
#define MESH_MAP 1
#define MESH_STATIC_OBJECT 2
#define MESH_DYNAMIC_OJBECT 3
#define MESH_MAP_UNDERWATER 5
#define MESH_STATIC_OBJECT_UNDERWATER 6
#define MESH_DYNAMIC_OJBECT_UNDERWATER 7

struct Terrain{

    bool isUnderwater;

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

    mat4 modelMatrix;
    int vertexCount;

    GLuint* causticTextureIds;
    int causticIndex = 0;
    double MAX_COUSTIC_TIME = 0.05f;
    double timer;
};

void terrainInit(Terrain* terrain, GLfloat* proj_mat, char* filename);
bool terrainLoadTerrainFile(const char *fileName, GLuint *vao, int *point_count);
void terrainSetInitialTransformation(Terrain* mesh, mat4* T, mat4* S,mat4* R );
void terrainLoadTexture(Terrain* mesh, char* filename);
void terrainLoadShaderProgram(Terrain * mesh);
void terrainGetUniforms(Terrain* mesh);
void terrainRender(Terrain* mesh, Camera* camera, GLfloat planeHeight, bool isAboveWater);
void terrainCleanUp(Terrain *mesh);
void terrainLoadCausticTexture(Terrain* mesh);
void terrainUpdate(Terrain *mesh, double elapsed_seconds);

#endif //AQUARIUM_TERRAIN_H
