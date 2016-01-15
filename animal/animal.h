//
// Created by alvaregd on 30/12/15.
//

#ifndef AQUARIUM_ANIMAL_H
#define AQUARIUM_ANIMAL_H

#include <GL/gl.h>
#include <utils/math_utils/maths_funcs.h>
#include <camera/camera.h>
#include <assimp/scene.h>

#define TERRAIN_TEXTURE "/home/alvaregd/Documents/Games/aquarium/assets/ambient_oclusion.png"
#define MESH_FILE "/home/alvaregd/Documents/Games/aquarium/assets/seagull.dae"
#define ANIMAL_VERTEX "/home/alvaregd/Documents/Games/aquarium/animal/mesh.vert"
#define ANIMAL_FRAGMENT "/home/alvaregd/Documents/Games/aquarium/animal/mesh.frag"

/* max bones allowed in a mesh */
#define MAX_BONES 32

/** describes animal's motion in the world*/
struct Transformation{

    vec3* posKeys;        //describes object's position at every key frame
    versor*rotKeys;       //describes object's orientation at every key frame
    vec3* scaleKeys;      //describes object's scale at every key frame
    double* posKeyTimes;  //key frame times for the position key frames
    double* rotKeyTimes;  //key frame times for the rotation key frames
    double* scaKeyTimes;  //key frame times for the scale key frames
    int numPosKeys;
    int numRotKeys;
    int numScaKeys;

    double animationDuration; //time it takes for animation to complete
    mat4 rotFix;              //fixes the orientation of the object t
    double transformation_time = 0.0; //time it takes for world animation (not the skeletal animation) to complete

};

/** Describes animal's bone/skin motion */
struct SkeletonNode{

    vec3* posKeys;
    versor* rotKeys;
    vec3* scaleKeys;
    double* posKeyTimes;
    double* rotKeyTimes;
    double* scaKeyTimes;
    int numPosKeys;
    int numRotKeys;
    int numScaKeys;

    SkeletonNode *children[MAX_BONES];
    char name[64];
     int numChildren;
    int boneIndex;

};

/** An animal object */
struct Animal{

    GLuint shader;
    GLuint texture;
    GLuint vao;
    GLuint vbo;

    GLint location_projection_mat;
    GLint location_view_mat;
    GLint location_model_mat;

    mat4 modelMatrix;

    //animal bone information
    int vertexCount;
    int boneCount;
    mat4* bone_offset_matrices;
    mat4* bone_animation_mats;
    int bone_matrices_location[MAX_BONES];
    SkeletonNode* nodes;
    float y = 0.0; // position of head

    //animation stuff
    double animationDuration;
    Transformation transformation;
    double anim_time = 0.0;

};

/** initialize the animal object, for now all settings are hardcoded in the function
 * This function will take care of all that is necessary to start using this animal object.
 * call animalRender after calling this function
 * in - animal object
 * in - projection matrix */
void animalInit(Animal* mesh, GLfloat* proj_mat);

/** loads .dae file into memory
 * in filename: name of the file containing the mesh information
 * in vao:  id of vertex array object to use with opengl
 * out point_count: number of vertices found in them mesh
 * out bone_offset_mats: address to store bone offset information
 * out boneCount: number of bones found
 * out rootNode: address of parent node
 * out animDuration: total animation duration time
 * returns true if function executed successfully */
bool animalLoadDaeFile(
        const char *fileName, GLuint *vao, int *point_count, mat4 *bone_offset_mats,
        int *boneCount, SkeletonNode **rootNode, double *animDuration);

/** get the mesh texture into memory from file
 * out mesh: load texture image for the mesh into memory*/
void animalLoadTexture(Animal* mesh);

/** load a shader program for the specified animal object
 *  for now the file location is hardcoded **/
void animalLoadShaderProgram(Animal * mesh);

/** get the shader uniforms **/
void animalGetUniforms(Animal* mesh);

/** render the animal
 * in animal: the animal object to render
 * in camera: the view camera*/
void animalRender(Animal* animal, Camera* camera);

 /** deletes the animal object and frees up memory*/
void animalCleanUp(Animal *mesh);

/** import the bones and its children
 * in assimpNode: address to assimp object containing node information
 * out skeletonNode: address to rootnode containing where we will update node information
 * in bonecount: number of expected bones to import
 * in boneNames: name of bone*/
bool animalImportSkeletonNode(aiNode* assimpNode, SkeletonNode **skeletonNode, int boneCount, char boneNames[][64]);

/** calculate the animal objects transformation for the next frame
 * in node: the node to perform calculations on
 * in animTime: the time progression of the animation
 * in parentMat: the current node's parent's transformation matrix
 * in boneOffsetMats: the bone's offset matrix
 * out boneAnimationmats: address to point to the updated transformation matrix of the current node**/
void animalSkeletonAnimate(Animal* mesh,
                         SkeletonNode* node,
                         double animTime,
                         mat4 parentMat,
                         mat4* boneOffsetMats,
                         mat4* boneAnimationMats);

/** finds a particular bone in the skeleton by traversing through the bone hierarchy
 * in root: the root node that is at the beginning of the traversal process
 * in nodeName: name of the node to retrieve
 * returns the address of the node we are looking for */
SkeletonNode* findNodeInSkeleton(SkeletonNode* root, const char* nodeName);

/** update animal object's movement */
void animalUpdate(Animal * animal, double elapsed_seconds);

#endif //AQUARIUM_ANIMAL_H
