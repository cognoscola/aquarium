//
// Created by alvaregd on 06/12/15.
//
/**
 * Mesh
 *
 * This object imports data from a .obj and creates a mesh to render to the scene.
 * Unlike an animal object which animates, a mesh object does not.
 */

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

/**
 * Initialize a mesh object.
 * out mesh: the object being initialized
 * in filename: uri of the .obj file containing mesh information
 * in texfilename: texture file that matches the .obj file
 * in proj_mat: a projection matrix
 */
void meshInit(Mesh* mesh, char* filename, char* texFilename ,GLfloat* proj_mat);

/**
 * loads a .obj file into memory
 * in filename: uri of the .obj file containing the mesh information
 * out vao: id of the vao object
 * out point_count: the number of vertices found in the mesh
 */
bool meshLoadMeshFile(const char *fileName, GLuint *vao, int *point_count);

/**
 * Creates a model matrix for the specified object
 * out mesh: the mesh object whose matrix is being calculated
 * in T: translation matrix
 * in S: scale matrix
 * in R: rotation matrix
 */
void meshSetInitialTransformation(Mesh* mesh, mat4* T, mat4* S,mat4* R );

/**
 * load a texture file into memory
 * out mesh: the mesh object to add the texture to
 * in filename: the uri of the texture file containing texture data
 */
void meshLoadTexture(Mesh* mesh, char* filename);

/**
 * creates a shader program for the mesh object
 * out mesh: the mesh object that will use the shader program
 */
void meshLoadShaderProgram(Mesh * mesh);

/**
 * fetch uniform locations in the mesh object's shader program
 */
void meshGetUniforms(Mesh* mesh);

/**
 * Renders the mesh object. Call this every frame
 * in mesh: the mesh object to render
 * in planeHeight: the clipping plane height, in case we need to render this object only partially
 * in isAboveWater: indicate wether this object is above or below water
 */
void meshRender(Mesh* mesh, Camera* camera, GLfloat planeHeight, bool isAboveWater);

/**
 * free memory used by this mesh object
 */
void meshCleanUp(Mesh *mesh);

/**
 * load a set of texture files for the purpose of creating caustic water effects
 */
void meshLoadCausticTexture(Mesh* mesh);

/**
 * update the mesh object
 * out mesh:
 * in elapsed_seconds: amount of time elapsed since program has started
 */
void meshUpdate(Mesh *mesh, double elapsed_seconds);
#endif //WATER_REFLECTION_MESH_H
