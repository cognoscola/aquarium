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

#define ANIMAL_VERTEX "/home/alvaregd/Documents/Games/aquarium/dynamic/mesh.vert"
#define ANIMAL_FRAGMENT "/home/alvaregd/Documents/Games/aquarium/dynamic/mesh.frag"

#define BONE_VERTEX "/home/alvaregd/Documents/Games/aquarium/dynamic/bone.vert"
#define BONE_FRAGMENT "/home/alvaregd/Documents/Games/aquarium/dynamic/bone.frag"

/* max bones allowed in a mesh */
#define MAX_BONES 32

/** describes animal's motion in the world*/
struct Transformation{

    vec3* posKeys;
    versor* rotKeys;
    vec3* scaleKeys;
    double* posKeyTimes;
    double* rotKeyTimes;
    double* scaKeyTimes;
    int numPosKeys;
    int numRotKeys;
    int numScaKeys;

    double animationDuration;
    mat4 rotFix;

};

/** Describes animal's motion */
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

struct Animal{

    GLuint shader;
    GLuint texture;
    GLuint vao;
    GLuint vbo;

    GLuint boneShader;
    GLuint boneVao;
    GLuint boneVbo;

    GLint location_projection_mat;
    GLint location_view_mat;
    GLint location_model_mat;

    GLint location_bone_view_mat;
    GLint location_bone_proj_mat;
    GLint location_bone_model_mat;

    mat4 modelMatrix;

    int vertexCount;
    int boneCount;
    mat4* monkey_bone_offset_matrices;
    mat4* monkey_bone_animation_mats;
    //rotation
    int bone_matrices_location[MAX_BONES];

    mat4 ear_mat;
    float theta = 0.0f;
    float rot_speed = 50.0f; // 50 radians per second

    SkeletonNode* nodes;

    float y = 0.0; // position of head

    //animation stuff
    double animationDuration;
};

void animalInit(Animal* mesh, GLfloat* proj_mat);
bool animalLoadAnimalFile(
        const char *fileName, GLuint *vao, int *point_count, mat4* bone_offset_mats,
        int* boneCount, SkeletonNode** rootNode, double* animDuration);
void animalLoadTexture(Animal* mesh);
void animalLoadShaderProgram(Animal * mesh);
void animalGetUniforms(Animal* mesh);
void animalRender(Animal* animal, Camera* camera);
void animalCleanUp(Animal *mesh);
bool animalImportSkeletonNode(aiNode* assimpNode, SkeletonNode **skeletonNode, int boneCount, char boneNames[][64]);
void animalSkeletonAnimate(Animal* mesh,
                         SkeletonNode* node,
                         double animTime,
                         mat4 parentMat,
                         mat4* boneOffsetMats,
                         mat4* boneAnimationMats);
SkeletonNode* findNodeInSkeleton(SkeletonNode* root, const char* nodeName);
void animalUpdate(Animal * animal, double time, Transformation* transformation, double transformationTime);



#endif //AQUARIUM_ANIMAL_H
