//
// Created by alvaregd on 26/12/15.
//
/**
 * Terrain
 *
 * Very similar to a Mesh object, with the exception of caustic textures.
 * A terrain object just reads mesh information into memory  from a .obj file and places it into the
 * scene. It also loads up caustic textures "frames" and mixes this texture with the terrain's texture
 * to create animated caustic lighting effects. Use this object in underwater scenes
 */

#ifndef AQUARIUM_TERRAIN_H
#define AQUARIUM_TERRAIN_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>
#include <camera/camera.h>

#define FLOOR_FILE "./assets/floor1.obj"
#define FLOOR_TEXTURE "./assets/terrain_texture.png"

#define MESH_TERRAIN_UNDER_VERTEX "./terrain/terrain_underwater.vert"
#define MESH_TERRAIN_UNDER_FRAG "./terrain/terrain_underwater.frag"


struct Terrain{

    GLuint shader;
    GLuint texture;
    GLuint vao;
    GLuint vbo;

    GLint location_projection_mat;   //projection matrix location
    GLint location_view_mat;         //view matrix location
    GLint location_model_mat;        //model matrix location
    GLint location_clip_plane;       // clipping plane location

    GLint location_baseTexture;      //base texture location
    GLint location_luminanceTexture; //caustic texture location

    GLint location_skyColour;        //allows use to modify the sky colour
    GLint location_fogDensity;
    GLint location_fogGradient;

    GLfloat fogDensity = 0.007f;
    GLfloat fogGradient = 1.5f;

    mat4 modelMatrix;
    int vertexCount;

    GLuint* causticTextureIds;      //list of caustic texture IDS
    int causticIndex = 0;           //indexer for caustic texture frames
    double MAX_COUSTIC_TIME = 0.05f;
    double timer;
};

/**
 * Initialize the specific terrain object. In this project there is only one terrain, so
 * most of the initial states are hardcoded. It is safe to call rerrainRender after this.
 * out terrain: the object to initialize
 * in project_mat: the projection matrix
 * int filename: the name of the file to import mesh data from
 */
void terrainInit(Terrain* terrain, GLfloat* proj_mat, char* filename);


/**
 * Loads a mesh file from memory and creates a vao out of it.
 * in filename: the file name to that contains the mesh information
 * out vao: location to store the vao ID
 * out point _count: the number of vertices found in the mesh
 * returns true if the upload process completed succesffully
 */
bool terrainLoadTerrainFile(const char *fileName, GLuint *vao, int *point_count);

/**
 * Set initial transformation of the mesh object
 * out mesh: the object to transform
 * in T: the matrix describing the translation portion of the transformation
 * in S: the matrix describing the scale portion of the transformation
 * in R: the matrix describing the rotation portion of the transformation
 */
void terrainSetInitialTransformation(Terrain* mesh, mat4* T, mat4* S,mat4* R );


/**
 * Load the object's texture information to memory
 * out mesh: the object that will use the texture
 * in filename: the filename of the file containing texture information
 */
void terrainLoadTexture(Terrain* mesh, char* filename);

/**
 * Load a terrain object's shader program
 */
void terrainLoadShaderProgram(Terrain * mesh);

/**
 * get the object's uniform locations from the object's shader
 */
void terrainGetUniforms(Terrain* mesh);

/**
 * Renderst he specified terrain object into the scene
 * in mesh: the terrain object to render
 * in camera: the camera of the scene
 * in planeHieght: the clipping plane height, to render the terrain partially
 * in isAboveWater: indicate wether the object above or below water.
 */
void terrainRender(Terrain* mesh, Camera* camera, GLfloat planeHeight, bool isAboveWater);


/**
 * free up memory taken by this object
 */
void terrainCleanUp(Terrain *mesh);

/**
 * load up caustic textures for the object. In this project there is only
 * one set of textures, so it is hardcoded for now
 */
void terrainLoadCausticTexture(Terrain* mesh);

/*
 * Update the terrain animation. In this project we are updating the caustic
 * texture frame
 * in mesh: the object to update
 * in elapsed_seconds: amount of time that passed since the program was started
 */
void terrainUpdate(Terrain *mesh, double elapsed_seconds);

#endif //AQUARIUM_TERRAIN_H
