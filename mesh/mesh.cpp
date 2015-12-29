
#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <utils/io/texture.h>
#include <utils/io/shader_loader.h>
#include <utils/io/texload.h>
#include "mesh.h"

void meshInit(Mesh* mesh, char* filename, char* texFilename ,GLfloat* proj_mat){

    assert(meshLoadMeshFile(filename, &mesh->vao, &mesh->vertexCount));
    if (texFilename != NULL) {
        meshLoadTexture(mesh, texFilename);
    }

    meshLoadShaderProgram(mesh);
    glUseProgram(mesh->shader);
    meshGetUniforms(mesh);
    glUniform4f(mesh->location_clip_plane, 0.0f, -1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(mesh->location_projection_mat , 1, GL_FALSE, proj_mat);
//
    mesh->modelMatrix = (mat4 *) malloc(mesh->numCopies * sizeof(mat4));
//    for (int i = 0; i < collection->numberOfCopies; i++) {
//        mesh->modelMatrix[i] = collection->T[i] * collection->S[i] * collection->R[i];
//    }
//
//    free(collection->T);
//    free(collection->R);
//    free(collection->S);
//    free(collection->objfilename);
//
//    //TODO mountain settings
////    mat4 s = scale(identity_mat4(), vec3(50,100,10));
////    mat4 T = translate(identity_mat4(), vec3(0.0f, -4.5f, -400.0f));
////    mesh->modelMatrix = T * s;
}

void meshSetInitialTransformation(Mesh* mesh, mat4* T, mat4* S,mat4* R ) {

    for (int i = 0; i < mesh->numCopies; i++) {
        mesh->modelMatrix[i] = T[i] * S[i] * R[i];
    }
//    glUniformMatrix4fv(mesh->location_model_mat , 1, GL_FALSE, mesh->modelMatrix.m);
}

bool meshLoadMeshFile(const char *fileName, GLuint *vao, int *point_count){

    const aiScene *scene = aiImportFile(fileName, aiProcess_Triangulate);
    if (!scene) {
        fprintf(stderr, "ERROR: reading mesh %s\n", fileName);
        return false;
    }

    printf(" %i animations\n", scene->mNumAnimations);
    printf(" %i Cameras\n", scene->mNumCameras);
    printf(" %i lights\n", scene->mNumLights);
    printf(" %i Materials\n", scene->mNumMaterials);
    printf(" %i Meshes\n", scene->mNumMeshes);
    printf(" %i textures\n", scene->mNumTextures);

    /*get the first mesh */
    const aiMesh* mesh = scene->mMeshes[0];
    printf("  %i vertices in mesh[0]\n", mesh->mNumVertices);

    /**pass back the mesh count */
    *point_count  = mesh->mNumVertices;

    /* generate */
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    /** make the data contiguous */
    GLfloat * points = NULL;
    GLfloat * normals = NULL;
    GLfloat * texcoords = NULL;

    if (mesh->HasPositions()) {
        points = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mVertices[i]);
            points[i * 3 + 0] = (GLfloat)vp->x;
            points[i * 3 + 1] = (GLfloat)vp->y;
            points[i * 3 + 2] = (GLfloat)vp->z;
        }
    }

    if (mesh->HasNormals()) {
        normals = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mNormals[i]);
            normals[i * 3 + 0] = (GLfloat)vp->x;
            normals[i * 3 + 1] = (GLfloat)vp->y;
            normals[i * 3 + 2] = (GLfloat)vp->z;

        }
    }

    if (mesh->HasTextureCoords(0)) {
        texcoords = (GLfloat *) malloc(*point_count * 2 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(mesh->mTextureCoords[0][i]);
            texcoords[i * 2 + 0] = (GLfloat)vp->x;
            texcoords[i * 2 + 1] = (GLfloat)vp->y;

        }
    }

    /** make vbos*/
    if (mesh->HasPositions()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        free(points);
    }

    if (mesh->HasNormals()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        free(normals);
    }

    if (mesh->HasTextureCoords(0)) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * *point_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        free(texcoords);
    }

    if (mesh->HasTangentsAndBitangents()) {

    }
    aiReleaseImport(scene);
    printf("Mesh loaded\n");
    return true;
}


void meshLoadTexture(Mesh* mesh, char* filename){

    GLuint texID;
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    unsigned char* image_data;
    int x ,y;
    loadImageFile(filename, true, &image_data, &x, &y);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    free(image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    mesh->texture = texID;
}

void meshLoadCausticTexture(Mesh* mesh) {

#define CAUSTIC "/home/alvaregd/Documents/Games/aquarium/assets/caustics/caust"

    mesh->causticTextureIds = (GLuint *) malloc(32 * sizeof(GLuint));

    for (int i = 0; i < 32; i++) {
        glGenTextures(1, &mesh->causticTextureIds[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->causticTextureIds[i]);

        char *name = (char *) malloc(100 * sizeof(char));
        sprintf(name, "%s%02d.bw", CAUSTIC, i);

        GLubyte *image_data;
        int x, y;

        image_data = read_alpha_texture(name, &x, &y);
        if (image_data == NULL) {
            fprintf(stderr, "\n%s: could not load caustic image file\n", CAUSTIC);
            exit(1);
        }
        free(name);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, x, y, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, image_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(image_data);
    }
}

void meshLoadShaderProgram(Mesh * mesh) {
    mesh->shader = create_programme_from_files(MESH_VERTEX, MESH_FRAGMENT);
}

void meshGetUniforms(Mesh* mesh){
    mesh->location_model_mat       = glGetUniformLocation(mesh->shader, "modelMatrix");
    mesh->location_view_mat        = glGetUniformLocation(mesh->shader, "viewMatrix");
    mesh->location_projection_mat  = glGetUniformLocation(mesh->shader, "projectionMatrix");
    mesh->location_clip_plane      = glGetUniformLocation(mesh->shader, "plane");

}

void meshUpdate(Mesh *mesh, double elapsed_seconds){

    mesh->timer += elapsed_seconds;
    if (mesh->timer > mesh->MAX_COUSTIC_TIME) {
        mesh->timer =0;

        mesh->causticIndex++;
        if (mesh->causticIndex > 31) {
            mesh->causticIndex = 0;
        }
    }
}

void meshRender(Mesh* mesh, Camera* camera, GLfloat planeHeight, bool isAboveWater) {

    glUseProgram(mesh->shader);
    glUniform4f(mesh->location_clip_plane, 0.0f, 1.0f, 0.0f, planeHeight);
    glUniformMatrix4fv(mesh->location_view_mat, 1, GL_FALSE, camera->viewMatrix.m);

    glBindVertexArray(mesh->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->texture);

    for (int i = 0; i < mesh->numCopies; i++) {
        glUniformMatrix4fv(mesh->location_model_mat, 1, GL_FALSE, mesh->modelMatrix[i].m);
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);

}

void meshCleanUp(Mesh *mesh){
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
}





