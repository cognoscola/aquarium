#include "main.h"

#include <skybox/skybox.h>
#include <platform/glfw_launcher.h>
#include <mesh/mesh.h>
#include <terrain/terrain.h>
#include <water/water.h>
#include <utils/io/video.h>
#include <ray/ray.h>
#include <animal/animal.h>
#include <glass/glass.h>

#define LINE_LENGTH 100
#define LANDSCAPE "/home/alvaregd/Documents/Games/aquarium/assets/plant_plan.txt"

void parseLine(char* line, MeshCollection*col);
int findLength(char *input);
void getValues(float*matInputs, char*stringValue);
void importMeshData(MeshCollection *collection, char *filename);
void initMeshCollection(MeshCollection*col, GLfloat* proj_mat);
void collectionRender(MeshCollection* col, Camera* camera, GLfloat planeHeight, bool isAboveWater);
void collectionCleanUp(MeshCollection* col);

int main() {

    Window hardware; //manage the window
    assert(restart_gl_log());  //restart the log system
    assert(start_gl(&hardware)); //start the glfw instance

    //check that our framebuffer is initialized correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Frame Buffer messed up\n");
    }

    //Assign the input callbacks
    glfwSetCursorPosCallback(hardware.window,cursor_position_callback);
    glfwSetInputMode(hardware.window, GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(hardware.window, key_callback);
    glfwSetInputMode(hardware.window,GLFW_STICKY_KEYS, 1);

    Input fogInputs;

    Video video; //image/video capture components
    reserve_video_memory (&video, &hardware);

    camera = {}; //camera components
    cameraInit(&camera, &hardware);

    Skybox sky; // sky object
    skyInit(&sky, camera.proj_mat);

    Ray ray;  // rays of light underwater
    rayInit(&ray, camera.proj_mat);

    Animal bird; //colourful bird that moves along the water surface
    animalInit(&bird, camera.proj_mat);

    Glass glass; //just for fun
    glassInit(&glass, &hardware, camera.proj_mat);

//    MeshCollection collection;
//    importMeshData(&collection, (char*)LANDSCAPE);
//    initMeshCollection(&collection,camera.proj_mat);

    Terrain terrain;
    terrainInit(&terrain, camera.proj_mat, (char*)FLOOR_FILE);
    mat4 S  = scale(identity_mat4(), vec3(4,2,4));
    mat4 T = translate(identity_mat4(), vec3(0.0f, -64.0f, 0.0f));
    mat4 R = identity_mat4();
    terrainSetInitialTransformation(&terrain, &T, &S, &R);

    Water water; // water object
    waterInit(&water, &hardware, camera.proj_mat);

    glEnable (GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    bool isBreaking = false;
    bool breakLatch = false;

    while(!glfwWindowShouldClose (hardware.window)) {

        //timing calculation
        static double previous_seconds = glfwGetTime ();
        double current_seconds = glfwGetTime ();
        double elapsed_seconds = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        if(videoUpdateTimer(&video, &elapsed_seconds)) break;

        glEnable(GL_CLIP_DISTANCE0);
        updateMovement(&camera, &input);
        terrainUpdate(&terrain, elapsed_seconds);

        bool isAboveWater = camera.pos[1] > water.waterHeight;

        //RENDER THE REFLECTION BUFFER
        bindFrameBufer(water.reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        water.cameraToWaterDistance = 2 * (camera.pos[1] - water.waterHeight); //save the camera height
        calculateRotationMatrix(-camera.pitch, &camera.Rpitch, PITCH);
        calculateViewMatrices(&camera);
        camera.viewMatrix.m[13] += (isAboveWater ? -1:1) *  water.cameraToWaterDistance;
//        collectionRender(&collection, &camera, (isAboveWater ? -1 : 1) * 1000.0f, isAboveWater);
        skyUpdate(&sky);
        skyRender(&sky, &camera, isAboveWater,true);
        animalRender(&bird, &camera);
        camera.viewMatrix.m[13] -=  (isAboveWater ? -1:1) * water.cameraToWaterDistance;
        calculateRotationMatrix(camera.pitch, &camera.Rpitch,PITCH);
        calculateViewMatrices(&camera);
        unbindCurrentFrameBuffer(&hardware);

        //RENDER THE REFRACTION BUFFER
        bindFrameBufer(water.refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        collectionRender(&collection, &camera, (!isAboveWater ? -1 : 1) * 1000.0f, isAboveWater);
        skyRender(&sky, &camera, camera.pos[1] > water.waterHeight,true);
        unbindCurrentFrameBuffer(&hardware);

        if(isBreaking && breakLatch) {
            glassBindFrameBufer(glass.framebuffer, GLASS_REFLECTION_WIDTH, GLASS_REFLECTION_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CLIP_DISTANCE0);
//            collectionRender(&collection, &camera, (isAboveWater ? -1 : 1) * 1000.0f, isAboveWater);
            if (!isAboveWater) {
                terrainRender(&terrain, &camera, 1000.0f, isAboveWater);
            }
            skyRender(&sky, &camera,isAboveWater,false);
            waterUpdate(&water);
            waterRender(&water, &camera);
            rayRender(&ray, &camera,isAboveWater, current_seconds,elapsed_seconds);
            animalRender(&bird, &camera);
            glassUnbindCurrentFrameBuffer(&hardware);
            isBreaking = false;
        }

        //RENDER TO THE DEFAULT BUFFER
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CLIP_DISTANCE0);
//        collectionRender(&collection, &camera, (isAboveWater ? -1 : 1) * 1000.0f, isAboveWater);
        if (!isAboveWater) {
            terrainRender(&terrain, &camera, 1000.0f, isAboveWater);
        }
        skyRender(&sky, &camera,isAboveWater,false);
        waterUpdate(&water);
        waterRender(&water, &camera);
        rayRender(&ray, &camera,isAboveWater, current_seconds,elapsed_seconds);
        animalRender(&bird, &camera);

        if(isBreaking || breakLatch) {
            glassRender(&glass, &camera, elapsed_seconds);
        }

        //handle various keyboard inputs
        glfwPollEvents();

        if (GLFW_PRESS == glfwGetKey (hardware.window, GLFW_KEY_P)) {
            video.dump_video = true;
            printf("Recording...");
        }
        if (GLFW_PRESS == glfwGetKey (hardware.window, GLFW_KEY_SPACE)) {
            assert (screencapture (&hardware));
        }
        if (GLFW_PRESS == glfwGetKey(hardware.window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(hardware.window, 1);
        }

        if (GLFW_PRESS== glfwGetKey(hardware.window, GLFW_KEY_PAGE_UP)) {
            if(!fogInputs.pageUpPressed){

                water.fogDensity += 0.0001f;
                printf(" Fog Density: %f\n", water.fogDensity);
                fogInputs.pageUpPressed = true;
            }
        }
        if (GLFW_RELEASE== glfwGetKey(hardware.window, GLFW_KEY_PAGE_UP)) {
            if (fogInputs.pageUpPressed) {
                fogInputs.pageUpPressed = false;
            }
        }

        if (GLFW_PRESS== glfwGetKey(hardware.window, GLFW_KEY_PAGE_DOWN)) {
            if (!fogInputs.pageDownPressed) {

                water.fogDensity -= 0.0001f;
                printf(" Fog Density: %f\n", water.fogDensity);
                fogInputs.pageDownPressed = true;
            }
        }
        if (GLFW_RELEASE== glfwGetKey(hardware.window, GLFW_KEY_PAGE_DOWN)) {
            if (fogInputs.pageDownPressed) {
                fogInputs.pageDownPressed = false;
            }
        }

        if (GLFW_PRESS == glfwGetKey(hardware.window, GLFW_KEY_HOME)) {
            if (!fogInputs.homePressed) {
                water.fogGradient += 1.0f;
                printf(" Fog Gradient: %f\n", water.fogGradient);
                fogInputs.homePressed = true;
            }
        }

        if (GLFW_RELEASE== glfwGetKey(hardware.window, GLFW_KEY_HOME)) {
            if (fogInputs.homePressed) {
                fogInputs.homePressed = false;
            }
        }

        if (GLFW_PRESS == glfwGetKey(hardware.window, GLFW_KEY_END)) {
            if (!fogInputs.endPressed) {
                water.fogGradient -= 1.0f;
                printf(" Fog Gradient: %f\n", water.fogGradient);
                fogInputs.endPressed = true;
            }
        }
        if (GLFW_RELEASE== glfwGetKey(hardware.window, GLFW_KEY_END)) {
            if (fogInputs.endPressed) {
                fogInputs.endPressed = false;
            }
        }

        if (GLFW_PRESS == glfwGetKey(hardware.window, GLFW_KEY_B)) {
            if (!breakLatch && !isBreaking) {
                isBreaking = true;
                breakLatch = true;
                glass.transitionTime = 0.0f;
            }
        }

        if(GLFW_RELEASE == glfwGetKey(hardware.window, GLFW_KEY_B)) {
            if(breakLatch) {
                breakLatch = false;
            }
        }

        animalUpdate(&bird,elapsed_seconds);
        if (video.dump_video) { // check if recording mode is enabled
            while (video.video_dump_timer > video.frame_time) {
                grab_video_frame (&video, &hardware); // 25 Hz so grab a frame
                video.video_dump_timer -= video.frame_time;
            }
        }

        glfwSwapBuffers(hardware.window);
    }

    waterCleanUp(&water);
    terrainCleanUp(&terrain);
    skyCleanUp(&sky);
//    collectionCleanUp(&collection);
    animalCleanUp(&bird);
    glassCleanUp(&glass);

    if(video.dump_video) {
        dump_video_frames(&video, &hardware);
    }
    /* close GL context and any other GLFW resources */
    glfwTerminate();
    return 0;
}

/**
 * Called when we press a key on the keyboard
 * in window - the focused window
 * in key    - which key?
 * in scancode
 * in action - One of GFLW_PRESS, GLFW_REPEAT or GLFW_RELEASE
 */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_W:
            input.wPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.wPressed);
            camera.move_angle =  action == GLFW_PRESS ? 0:camera.move_angle;
            break;
        case GLFW_KEY_S:
            input.sPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.sPressed);
            camera.move_angle =  action == GLFW_PRESS ? 180:camera.move_angle;
            break;
        case GLFW_KEY_A:
            input.aPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.aPressed);
            camera.move_angle =  action == GLFW_PRESS ? -90:camera.move_angle;
            break;
        case GLFW_KEY_D:
            input.dPressed = action == GLFW_PRESS ? true: ((action == GLFW_RELEASE) ? false: input.dPressed);
            camera.move_angle =  action == GLFW_PRESS ? 90:camera.move_angle;
            break;
    }
}

/**
 * Called every time the cursor moves. It is used to calculate the Camera's direction
 * in window - the window holding the cursor
 * in xpos   - the xposition of the cursor on the screen
 * in ypos   - the yposition of the curose on the screen
 */
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {

    //calculate pitch
    static double  previous_ypos = ypos;
    double position_y_difference = ypos - previous_ypos;
    previous_ypos = ypos;

    //calculate yaw
    static double previous_xpos = xpos;
    double position_x_difference = xpos - previous_xpos;
    previous_xpos = xpos;

    //reduce signal
    camera.yaw += position_x_difference *camera.signal_amplifier;
    camera.pitch += position_y_difference *camera.signal_amplifier;

    //calculate rotation sequence
    calculateRotationMatrix(camera.pitch, &camera.Rpitch, PITCH);
    calculateRotationMatrix(camera.yaw, &camera.Ryaw, YAW);
}

void importMeshData(MeshCollection *collection, char *filename){

    //reset collection data;
    collection->fileIndex = -1;
    collection->hasT = false;
    collection->hasS = false;
    collection->hasR = false;
    collection->meshObject = NULL;

    FILE* file = fopen (filename , "r");
    if (!file) {
        gl_log_err ("ERROR: opening file for reading: %s\n", filename);
        exit(1);
    }else{

    }

    int current_len = 0;
    char line[2048];
    strcpy (line, ""); // remember to clean up before using for first time!
    while (!feof (file)) {
        if (NULL != fgets (line, 2048, file)) {
            parseLine(line, collection);
            strcpy(line, ""); // remember to clean up before using for first time!
            current_len += strlen(line); // +1 for \n at end
            if (current_len >= 100) {
                gl_log_err(
                        "ERROR: shader length is longer than string buffer length %i\n",
                        100
                );
            }
        }
    }

    if (EOF == fclose (file)) { // probably unnecesssary validation
        gl_log_err ("ERROR: closing file from reading %s\n", filename);
    }
}

void parseLine(char* line, MeshCollection*col) {

    if (strstr(line, "-N") != NULL) {
        size_t length =(size_t)findLength(line) - 3;
        char* value = (char*)malloc(length * sizeof(char));
        memcpy(value,&line[3],length);
        col->numberOfFiles =  atoi(value);
        col->meshObject = (MeshObject *) malloc(col->numberOfFiles * sizeof(MeshObject));

        for (int i = 0; i < col->numberOfFiles; i++) {
            col->meshObject[i].index = 0;
        }
        printf("Allocating new mesh space for %i files\n", col->numberOfFiles);
    }

    if (strstr(line, "-f") != NULL) {

        col->fileIndex++;
        size_t length =(size_t) findLength(line) - 3;
        printf("Name Length: %i\n", length);
//        col->meshObject[col->fileIndex].objfilename = (char *) malloc(length * sizeof(char));
        memcpy(col->meshObject[col->fileIndex].objfilename, &line[3], length);

        printf("File: %s\n", col->meshObject[col->fileIndex].objfilename);
    }

    if (strstr(line, "-n") != NULL) {

        size_t length =(size_t)findLength(line) - 3;
        char* value = (char*)malloc(length * sizeof(char));
        memcpy(value,&line[3],length);
        col->meshObject[col->fileIndex].numberOfCopies = atoi(value);
        printf("Copies:%i\n", col->meshObject[col->fileIndex].numberOfCopies);

        col->meshObject[col->fileIndex].T =
                (mat4 *) malloc(col->meshObject[col->fileIndex].numberOfCopies * sizeof(mat4));
        col->meshObject[col->fileIndex].R =
                (mat4 *) malloc(col->meshObject[col->fileIndex].numberOfCopies * sizeof(mat4));
        col->meshObject[col->fileIndex].S =
                (mat4 *) malloc(col->meshObject[col->fileIndex].numberOfCopies * sizeof(mat4));
    }

    if (strstr(line, "-r") != NULL) {
        col->hasR = false;
    }

    if (strstr(line, "-t") != NULL) {
        float input[3];
        getValues(input, line);

        MeshObject* obj =&col->meshObject[col->fileIndex];
        obj->T[obj->index] = translate(identity_mat4(), vec3(input[0], input[1], input[2]));
        col->hasT = true;
    }

    if (strstr(line, "-s") != NULL) {

        float input[3];
        getValues(input, line);

        MeshObject* obj =&col->meshObject[col->fileIndex];

        obj->S[obj->index] = scale(identity_mat4(), vec3(input[0], input[1], input[2]));
        col->hasS = true;

        //check that all matrices have been instantiated
        if (!col->hasR) {
            printf("created identity mat for R at %i\n", obj->index);
            obj->R[obj->index] = identity_mat4();
        }

        if (!col->hasT) {
            obj->T[obj->index] = identity_mat4();
        }
        //todo for now this is always true
        if (!col->hasS) {
            obj->S[obj->index] = identity_mat4();
        }

        obj->index++;
        col->hasR = false;
        col->hasS = false;
        col->hasT = false;
    }

}

int findLength(char *input){

    int length = 0;
    while(input[length]!='\n')  //  remove ;
    {
        length++;
    }
    return length;
}

void getValues(float* matInputs, char* line){

    printf("getValue() Input: %s", line);
    size_t length = (size_t)findLength(line) - 3;
    printf("getValue() Length: %i\n", length);
    char stringValue[length];
    memset(stringValue, 0, sizeof stringValue);
    memcpy(stringValue, &line[3], length);
    printf("getValue() after Copy: %s\n", stringValue);

    char seps[] = ",";
    char* token;
    float var;
    int i = 0;

    token = strtok (stringValue, seps);
    while (token != NULL)
    {
        sscanf (token, "%f", &var);
        matInputs[i++] = var;

        printf("Value: %f\n", var);
        token = strtok (NULL, seps);
    }

//    free(stringValue);
}

void initMeshCollection(MeshCollection* col, GLfloat* proj_mat){

    //for object inside the collection, initialize a new Mesh item
    col->mesh = (Mesh*)malloc(sizeof(Mesh) * col->numberOfFiles);

    for (int i = 0; i < col->numberOfFiles; i++) {
        col->mesh[i].numCopies = col->meshObject[i].numberOfCopies;
        meshInit(&col->mesh[i], col->meshObject[i].objfilename, NULL, proj_mat);
        meshSetInitialTransformation(&col->mesh[i], col->meshObject[i].T,col->meshObject[i].S,col->meshObject[i].R);
    }
    free(col->meshObject);
}

void collectionRender(MeshCollection* col, Camera* camera, GLfloat planeHeight, bool isAboveWater) {

    for (int i = 0; i < col->numberOfFiles; i++) {
        meshRender(&col->mesh[i], camera, planeHeight, isAboveWater);
    }
}

void collectionCleanUp(MeshCollection* col) {

    for (int i = 0; i < col->numberOfFiles; i++) {
        meshCleanUp(&col->mesh[i]);
    }
//    free(col);
}


