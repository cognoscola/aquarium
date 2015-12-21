#include "main.h"

#include <skybox/skybox.h>
#include <platform/glfw_launcher.h>
#include <mesh/mesh.h>
#include <water/water.h>
#include <utils/io/video.h>

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
    glfwSetInputMode(hardware.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(hardware.window, key_callback);
    glfwSetInputMode(hardware.window,GLFW_STICKY_KEYS, 1);

    Input fogInputs;

    Video video; //image/video capture components
    reserve_video_memory (&video, &hardware);

    camera = {}; //camera components
    cameraInit(&camera, &hardware);

    Skybox sky; // sky object
    skyInit(&sky, camera.proj_mat);

    Mesh terrain; // terrain object
    meshInit(&terrain, camera.proj_mat);

    Mesh map;
    meshInit(&map, camera.proj_mat);

    Water water; // water object
    waterInit(&water, &hardware, camera.proj_mat);

    glEnable (GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    while(!glfwWindowShouldClose (hardware.window)) {

        //timing calculation
        static double previous_seconds = glfwGetTime ();
        double current_seconds = glfwGetTime ();
        double elapsed_seconds = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        if(videoUpdateTimer(&video, &elapsed_seconds)) break;

        glEnable(GL_CLIP_DISTANCE0);
        updateMovement(&camera, &input);


        //RENDER THE REFLECTION BUFFER
        bindFrameBufer(water.reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        water.reflectionDistance = 2 * (camera.pos[1] - water.waterHeight); //save the camera height
        calculateRotationMatrix(-camera.pitch, &camera.Rpitch, PITCH);
        calculateViewMatrices(&camera);
        camera.viewMatrix.m[13] += (camera.pos[1] > water.waterHeight ? -1:1) *  water.reflectionDistance;
//        meshRender(&terrain,&camera,0.5);
        meshRender(&map, &camera,(camera.pos[1] > water.waterHeight ? -1:1) * 0.5f);
        skyUpdate(&sky);
        skyRender(&sky, &camera, camera.pos[1] > water.waterHeight);
        camera.viewMatrix.m[13] -=  (camera.pos[1] > water.waterHeight ? -1:1) * water.reflectionDistance;
        calculateRotationMatrix(camera.pitch, &camera.Rpitch,PITCH);
        calculateViewMatrices(&camera);
        unbindCurrentFrameBuffer(&hardware);

        //RENDER THE REFRACTION BUFFER
        bindFrameBufer(water.refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        meshRender(&terrain,&camera, 5.0f);
        meshRender(&map, &camera, (camera.pos[1] > water.waterHeight ? -1:1) * 1000.0f);
        skyRender(&sky, &camera, camera.pos[1] > water.waterHeight);
        unbindCurrentFrameBuffer(&hardware);

        //RENDER TO THE DEFAULT BUFFER
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CLIP_DISTANCE0);
//        meshRender(&terrain,&camera,1000.0f);
        meshRender(&map, &camera, (camera.pos[1] > water.waterHeight ? -1:1) * 1000.0f);
        skyRender(&sky, &camera,camera.pos[1] > water.waterHeight);
        waterUpdate(&water);
        waterRender(&water, &camera);

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

                water.fogDensity += 0.001f;
                printf("Water Fog Density: %f\n", water.fogDensity);
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
                printf("Water Fog Density: %f\n", water.fogDensity);
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
                printf("Water Fog Gradient: %f\n", water.fogGradient);
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
                printf("Water Fog Gradient: %f\n", water.fogGradient);
                fogInputs.endPressed = true;
            }
        }
        if (GLFW_RELEASE== glfwGetKey(hardware.window, GLFW_KEY_END)) {
            if (fogInputs.endPressed) {
                fogInputs.endPressed = false;
            }
        }


        if (video.dump_video) { // check if recording mode is enabled
            while (video.video_dump_timer > video.frame_time) {
                grab_video_frame (&video, &hardware); // 25 Hz so grab a frame
                video.video_dump_timer -= video.frame_time;
            }
        }
        glfwSwapBuffers(hardware.window);
    }

    waterCleanUp(&water);
    meshCleanUp(&terrain);
    skyCleanUp(&sky);

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








