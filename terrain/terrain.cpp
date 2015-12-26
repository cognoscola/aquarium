//
// Created by alvaregd on 26/12/15.
//

#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <utils/io/texture.h>
#include <utils/io/shader_loader.h>
#include <utils/io/texload.h>
#include "terrain.h"

void terrainInit(Terrain* terrain, GLfloat* proj_mat, char* filename){

    assert(terrainLoadTerrainFile(filename, &terrain->vao, &terrain->vertexCount));

    terrainLoadTexture(terrain, FLOOR_TEXTURE);
    terrainLoadCausticTexture(terrain);

    terrainLoadShaderProgram(terrain);
    glUseProgram(terrain->shader);
    terrainGetUniforms(terrain);
    glUniform4f(terrain->location_clip_plane, 0.0f, -1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(terrain->location_projection_mat , 1, GL_FALSE, proj_mat);

    glUniform1i(terrain->location_baseTexture, 0);
    glUniform1i(terrain->location_luminanceTexture,1 );

    glUniform1f(terrain->location_fogDensity, terrain->fogDensity);
    glUniform1f(terrain->location_fogGradient,terrain->fogGradient);
    glUniform3f(terrain->location_skyColour,0.0f,0.7f,1.0f);


}

void terrainSetInitialTransformation(Terrain* terrain, mat4* T, mat4* S,mat4* R ){
    terrain->modelMatrix = *T * *S * *R;
    glUniformMatrix4fv(terrain->location_model_mat , 1, GL_FALSE, terrain->modelMatrix.m);
}



bool terrainLoadTerrainFile(const char *fileName, GLuint *vao, int *point_count){

    const aiScene *scene = aiImportFile(fileName, aiProcess_Triangulate);
    if (!scene) {
        fprintf(stderr, "ERROR: reading terrain %s\n", fileName);
        return false;
    }

    printf(" %i animations\n", scene->mNumAnimations);
    printf(" %i Cameras\n", scene->mNumCameras);
    printf(" %i lights\n", scene->mNumLights);
    printf(" %i Materials\n", scene->mNumMaterials);
    printf(" %i Meshes\n", scene->mNumMeshes);
    printf(" %i textures\n", scene->mNumTextures);

    /*get the first terrain */
    const aiMesh* terrain = scene->mMeshes[0];
    printf("  %i vertices in terrain[0]\n", terrain->mNumVertices);

    /**pass back the terrain count */
    *point_count  = terrain->mNumVertices;

    /* generate */
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    /** make the data contiguous */
    GLfloat * points = NULL;
    GLfloat * normals = NULL;
    GLfloat * texcoords = NULL;

    if (terrain->HasPositions()) {
        points = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(terrain->mVertices[i]);
            points[i * 3 + 0] = (GLfloat)vp->x;
            points[i * 3 + 1] = (GLfloat)vp->y;
            points[i * 3 + 2] = (GLfloat)vp->z;
        }
    }

    if (terrain->HasNormals()) {
        normals = (GLfloat *) malloc(*point_count * 3 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(terrain->mNormals[i]);
            normals[i * 3 + 0] = (GLfloat)vp->x;
            normals[i * 3 + 1] = (GLfloat)vp->y;
            normals[i * 3 + 2] = (GLfloat)vp->z;

        }
    }

    if (terrain->HasTextureCoords(0)) {
        texcoords = (GLfloat *) malloc(*point_count * 2 * sizeof(GLfloat));
        for (int i = 0; i < *point_count; ++i) {
            const aiVector3D *vp = &(terrain->mTextureCoords[0][i]);
            texcoords[i * 2 + 0] = (GLfloat)vp->x;
            texcoords[i * 2 + 1] = (GLfloat)vp->y;

        }
    }

    /** make vbos*/
    if (terrain->HasPositions()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        free(points);
    }

    if (terrain->HasNormals()) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * *point_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        free(normals);
    }

    if (terrain->HasTextureCoords(0)) {
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * *point_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        free(texcoords);
    }

    if (terrain->HasTangentsAndBitangents()) {

    }
    aiReleaseImport(scene);
    printf("Terrain loaded\n");
    return true;
}


void terrainLoadTexture(Terrain* terrain, char* filename){

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
    terrain->texture = texID;
}

void terrainLoadCausticTexture(Terrain* terrain) {

#define CAUSTIC "/home/alvaregd/Documents/Games/aquarium/assets/caustics/caust"

    terrain->causticTextureIds = (GLuint *) malloc(32 * sizeof(GLuint));

    for (int i = 0; i < 32; i++) {
        glGenTextures(1, &terrain->causticTextureIds[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrain->causticTextureIds[i]);

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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, x, y, 0,GL_LUMINANCE, GL_UNSIGNED_BYTE, image_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        free(image_data);
    }
}

void terrainLoadShaderProgram(Terrain * terrain) {
    terrain->shader = create_programme_from_files(MESH_TERRAIN_UNDER_VERTEX, MESH_TERRAIN_UNDER_FRAG);
}

void terrainGetUniforms(Terrain* terrain){
    terrain->location_model_mat       = glGetUniformLocation(terrain->shader, "modelMatrix");
    terrain->location_view_mat        = glGetUniformLocation(terrain->shader, "viewMatrix");
    terrain->location_projection_mat  = glGetUniformLocation(terrain->shader, "projectionMatrix");
    terrain->location_clip_plane      = glGetUniformLocation(terrain->shader, "plane");
    terrain->location_baseTexture      = glGetUniformLocation(terrain->shader, "baseMap");
    terrain->location_luminanceTexture = glGetUniformLocation(terrain->shader, "luminanceMap");
    terrain->location_skyColour        = glGetUniformLocation(terrain->shader, "skyColour");
    terrain->location_fogDensity        = glGetUniformLocation(terrain->shader, "fogDensity");
    terrain->location_fogGradient       = glGetUniformLocation(terrain->shader, "fogGradient");
}

void terrainUpdate(Terrain *terrain, double elapsed_seconds){

    terrain->timer += elapsed_seconds;
    if (terrain->timer > terrain->MAX_COUSTIC_TIME) {
        terrain->timer =0;

        terrain->causticIndex++;
        if (terrain->causticIndex > 31) {
            terrain->causticIndex = 0;
        }
    }
}

void terrainRender(Terrain* terrain, Camera* camera, GLfloat planeHeight, bool isAboveWater) {

    glUseProgram(terrain->shader);
    glUniform4f(terrain->location_clip_plane, 0.0f, 1.0f, 0.0f, planeHeight);
    glUniformMatrix4fv(terrain->location_view_mat, 1, GL_FALSE, camera->viewMatrix.m);
    glUniformMatrix4fv(terrain->location_model_mat, 1, GL_FALSE, terrain->modelMatrix.m);

    glBindVertexArray(terrain->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    if (isAboveWater) {
        glUniform3f(terrain->location_skyColour, 0.6f, 0.6f, 0.6f);
    } else {
        terrain->fogDensity = 0.007f;
        terrain->fogGradient = 1.5f;
        glUniform3f(terrain->location_skyColour, 0.0f, 0.7f, 1.0f);
    }
    glUniform1f(terrain->location_fogDensity, terrain->fogDensity);
    glUniform1f(terrain->location_fogGradient, terrain->fogGradient);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrain->texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, terrain->causticTextureIds[terrain->causticIndex]);
    glDrawArrays(GL_TRIANGLES, 0, terrain->vertexCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);

}

void terrainCleanUp(Terrain *terrain){
    glDeleteVertexArrays(1, &terrain->vao);
    glDeleteBuffers(1, &terrain->vbo);
}



