/* Kelly Perez
CS 330
 Final Project
 Updated the look of the monitor, added directional camera movements, textured, and added lighting
 I took a few creative liberties and added in a couple objects that were not in the proposal, so I could fill out the scene a little better. 
 IE: Coffee cup and PC speakers
 Added shader file to reduce Source file length. */


#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h" // Camera class
#include "meshes.h" // Mesh class
#include "shader.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std; // Standard namespace


/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
bool isPerspective = true;
float gCameraSpeed = 2.5f;


// Main Program
int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    meshes.CreateMeshes();

    // Create the shader program

    if (!UCreateShaderProgram(surfaceVertexShaderSource, surfaceFragmentShaderSource, gSurfaceProgramId))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, gLightProgramId))
        return EXIT_FAILURE;

    // Load and Create Texture
    if (!UCreateTexture("monitor.jpg", gMonitor)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }

    if (!UCreateTexture("desk.jpg", gDesk)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("smiley.jpg", gCube)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;

    }
    if (!UCreateTexture("coffeecup.jpg", gCoffee)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("coffee2.jpg", gCoffee2)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("deskpad.jpg", gDeskpad)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("keyboard3.jpg", gKeyboard)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("mouse.jpg", gMouse)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("speaker.jpg", gSpeaker)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("speakerbox.jpg", gSpeakerBox)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }
    if (!UCreateTexture("bezel.jpg", gFrame)) {
        std::cerr << "Error creating texture" << std::endl;
        return EXIT_FAILURE;
    }




    glUseProgram(gSurfaceProgramId);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "uTextureBase"), 0);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "uTextureExtra"), 1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(gWindow)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        UProcessInput(gWindow);
        URender();
        glfwPollEvents();
    }

    meshes.DestroyMeshes();
    UDestroyShaderProgram(gSurfaceProgramId);
    UDestroyShaderProgram(gLightProgramId);
    exit(EXIT_SUCCESS);
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);


    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        gCameraSpeed += 0.1f;
        std::cout << "Increased Speed: " << gCameraSpeed << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        gCameraSpeed -= 0.1f;
        if (gCameraSpeed <= 0.1f)
            gCameraSpeed = 0.1f;
        std::cout << "Decreased Speed: " << gCameraSpeed << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime * gCameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime * gCameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime * gCameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime * gCameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime * gCameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime * gCameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        isPerspective = !isPerspective;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}




// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = static_cast<float>(xpos); 
        gLastY = static_cast<float>(ypos); 
        gFirstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - gLastX;
    float yoffset = gLastY - static_cast<float>(ypos); 

    gLastX = static_cast<float>(xpos); 
    gLastY = static_cast<float>(ypos); 

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}



// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0) // Scrolling up
    {
        gCameraSpeed += 0.5f;
    }
    else if (yoffset < 0) // Scrolling down
    {
        gCameraSpeed -= 0.5f;
        if (gCameraSpeed <= 0.1f)
            gCameraSpeed = 0.1f;
    }
    std::cout << "Camera Speed: " << gCameraSpeed << std::endl;
}




// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}




// Function called to render a frame
void URender()
{
    
    glm::mat4 translation;
    glm::mat4 projection2;

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint modelLoc;
    GLint viewLoc;
    GLint projLoc;
    GLint viewPosLoc;
    GLint ambStrLoc;
    GLint ambColLoc;
    GLint light1ColLoc;
    GLint light1PosLoc;
    GLint light2ColLoc;
    GLint light2PosLoc;
    GLint objColLoc;
    GLint specInt1Loc;
    GLint highlghtSz1Loc;
    GLint specInt2Loc;
    GLint highlghtSz2Loc;
    GLint uHasTextureLoc;
    bool ubHasTextureVal;
    glm::mat4 scale;
    glm::mat4 rotation;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;


    // Ortho and Perspective views
    if (isPerspective) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        projection2 = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
        projection2 = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }

    view = gCamera.GetViewMatrix();


    // Set the shader to be used
    glUseProgram(gSurfaceProgramId);
    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gSurfaceProgramId, "model");
    viewLoc = glGetUniformLocation(gSurfaceProgramId, "view");
    projLoc = glGetUniformLocation(gSurfaceProgramId, "projection");
    viewPosLoc = glGetUniformLocation(gSurfaceProgramId, "viewPosition");
    ambStrLoc = glGetUniformLocation(gSurfaceProgramId, "ambientStrength");
    ambColLoc = glGetUniformLocation(gSurfaceProgramId, "ambientColor");
    light1ColLoc = glGetUniformLocation(gSurfaceProgramId, "light1Color");
    light1PosLoc = glGetUniformLocation(gSurfaceProgramId, "light1Position");
    light2ColLoc = glGetUniformLocation(gSurfaceProgramId, "light2Color");
    light2PosLoc = glGetUniformLocation(gSurfaceProgramId, "light2Position");
    objColLoc = glGetUniformLocation(gSurfaceProgramId, "objectColor");
    specInt1Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity1");
    highlghtSz1Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize1");
    specInt2Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity2");
    highlghtSz2Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize2");
    uHasTextureLoc = glGetUniformLocation(gSurfaceProgramId, "ubHasTexture");

    // Get the uniform locations
    GLint keyLightIntensityLoc = glGetUniformLocation(gSurfaceProgramId, "keyLightIntensity");
    GLint fillLightIntensityLoc = glGetUniformLocation(gSurfaceProgramId, "fillLightIntensity");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the camera view location
    glUniform3f(viewPosLoc, gCamera.Position.x, gCamera.Position.y, gCamera.Position.z);

    //set ambient intensity
    glUniform1f(ambStrLoc, 0.5f);

    //set ambient color
    glUniform3f(ambColLoc, 0.1f, 0.1f, 0.1f);
    glUniform3f(light1ColLoc, 1.0f, 0.8f, 0.0f); // Orange Light
    glUniform3f(light1PosLoc, -15.0f, 5.0f, 8.0f);
    glUniform3f(light2ColLoc, 1.0f, 1.0f, 1.0f); // White Light
    glUniform3f(light2PosLoc, 0.0f, 5.0f, 3.0f);

    //set specular intensity
    glUniform1f(specInt1Loc, 0.1f);
    glUniform1f(specInt2Loc, 0.5f);

    //set specular highlight size
    glUniform1f(highlghtSz1Loc, 1.0f);
    glUniform1f(highlghtSz2Loc, 10.0f);

    // Set the intensity of key and fill light
    glUniform1f(keyLightIntensityLoc, 0.1);
    glUniform1f(fillLightIntensityLoc, 1.0);

    ubHasTextureVal = true;
    glUniform1i(uHasTextureLoc, ubHasTextureVal);


    // Activate the cube VAO (used by cube and lamp)
    glBindVertexArray(meshes.gBoxMesh.vao);
    glActiveTexture(GL_TEXTURE0);


    // CUBE: draw cube
    //----------------
    // Set the shader to be used
    glUseProgram(gSurfaceProgramId);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gSurfaceProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gSurfaceProgramId, "lightColor");
    GLint lightColorLoc2 = glGetUniformLocation(gSurfaceProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gSurfaceProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gSurfaceProgramId, "viewPosition");
    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    GLint UVScaleLoc = glGetUniformLocation(gSurfaceProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
    // bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gCube);
    // Draws the triangles
    //glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);



    // LAMP: draw lamp 1
    //----------------
    glUseProgram(gLightProgramId);
    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition) * glm::scale(gLightScale);
    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLightProgramId, "model");
    viewLoc = glGetUniformLocation(gLightProgramId, "view");
    projLoc = glGetUniformLocation(gLightProgramId, "projection");
    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    //glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

    // Deactivate the Vertex Array Object and shader program
    //glBindVertexArray(0);
    //glUseProgram(0);



    // LAMP: draw lamp 2
//----------------
    glUseProgram(gLightProgramId);
    //Transform the smaller cube used as a visual que for the light source
    model = glm::translate(gLightPosition2) * glm::scale(gLightScale2);
    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLightProgramId, "model");
    viewLoc = glGetUniformLocation(gLightProgramId, "view");
    projLoc = glGetUniformLocation(gLightProgramId, "projection");
    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    //glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);
    // Set the shader to be used
    glUseProgram(gSurfaceProgramId);
    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gSurfaceProgramId, "model");
    viewLoc = glGetUniformLocation(gSurfaceProgramId, "view");
    projLoc = glGetUniformLocation(gSurfaceProgramId, "projection");
    objectColorLoc = glGetUniformLocation(gSurfaceProgramId, "uObjectColor");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Deskpad
// Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gPlaneMesh.vao);
    // Activate and Bind the Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDeskpad);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(6.0f, 1.0f, 3.0f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(0.5f, -0.9f, 2.0f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color
    //glProgramUniform4f(gProgramId, objectColorLoc, 0.8f, 0.8f, 0.8f, 1.0f);
    // Draws the triangles
    glDrawElements(GL_TRIANGLES, meshes.gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    // Desk
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gPlaneMesh.vao);
    // Activate and Bind the Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDesk);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(8.0f, 1.0f, 6.0f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(0.5f, -1.0f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.6f, 0.4f, 0.2f, 1.0f);
    // Draws the triangles
    glDrawElements(GL_TRIANGLES, meshes.gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    // Cube Monitor
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gBoxMesh.vao);
    // Activate and Bind the Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMonitor);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(10.0f, 6.0f, 0.5f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(0.5f, 5.0f, -2.89f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the object
    //glProgramUniform4f(gCubeProgramId, objectColorLoc, 0.0f, 1.0f, 1.0f, 1.0f);
    // Draws the triangles
    glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);



    // Cube Monitor Frame
// Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gBoxMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gFrame);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(10.5f, 6.5f, 0.5f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(0.5f, 5.0f, -2.9f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the object
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
    // Draws the triangles
    glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);




    // Pyramid Monitor Stand
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gPyramid4Mesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gFrame);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(6.0f, 1.0f, 1.0f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(0.5f, -0.4f, -3.0f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the object
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid4Mesh.nVertices);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    // Cylinder Monitor Stand
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gCylinderMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gFrame);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.3f, 5.0f, 0.3f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(0.5f, -0.4f, -3.0f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the object
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);


    // Draws the triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
    glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
    glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Coffee Cup Body
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gCylinderMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCoffee);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.54f, 0.84f, 0.54f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(-3.5f, -1.0f, 3.5f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //glProgramUniform4f(gProgramId, objectColorLoc, 1.0f, 1.0f, 0.0f, 1.0f);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
    //glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
    glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Coffee Handle
// Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gTorusMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCoffee);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.33f, 0.3f, 0.3f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(-3.0f, -0.53f, 3.6f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.0f, 0.0f, 1.0f, 1.0f);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, meshes.gTorusMesh.nVertices);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Coffee
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gCylinderMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gCoffee2);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.53f, 0.67f, 0.53f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(-3.5f, -0.87f, 3.5f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //glProgramUniform4f(gProgramId, objectColorLoc, 1.0f, 1.0f, 0.0f, 1.0f);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
    glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
    glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Speaker 1
// Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gCylinderMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSpeaker);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.53f, 0.67f, 0.53f));
    // 2. Rotate the object
    rotation = glm::rotate(90.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(-4.0f, 0.0f, -3.5f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //glProgramUniform4f(gProgramId, objectColorLoc, 1.0f, 1.0f, 0.0f, 1.0f);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
    glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
    glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Speaker Box 1
// Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gBoxMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSpeakerBox);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(1.4f, 2.5f, 1.0f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(-4.0f, 0.0f, -3.4f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the object
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
    // Draws the triangles
    glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Speaker 2
// Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gCylinderMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSpeaker);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.53f, 0.67f, 0.53f));
    // 2. Rotate the object
    rotation = glm::rotate(90.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(5.0f, 0.0f, -3.5f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //glProgramUniform4f(gProgramId, objectColorLoc, 1.0f, 1.0f, 0.0f, 1.0f);
    // Draws the triangles
    glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
    glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
    glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Speaker Box 2
// Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gBoxMesh.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSpeakerBox);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(1.4f, 2.5f, 1.0f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(5.0f, 0.0f, -3.4f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the object
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
    // Draws the triangles
    glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Cube 2 Keyboard
    glBindVertexArray(meshes.gBoxMesh.vao);
    // Activate and Bind the Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gKeyboard);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(4.0f, 0.1f, 2.0f));
    // 2. Rotate the object
    rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(0.5f, -0.9f, 3.0f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the object
    //glProgramUniform4f(gProgramId, objectColorLoc, 0.0f, 0.75f, 0.8f, 1.0f);
    // Draws the triangles
    glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    // Mouse
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(meshes.gCylinderMesh.vao);
    // Activate and Bind the Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMouse);
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "texture"), 0); // bind the texture to the shader
    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.5f, 0.1f, 0.3f));
    // 2. Rotate the object
    rotation = glm::rotate(39.3f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Position the object
    translation = glm::translate(glm::vec3(5.0f, -0.9f, 3.0f));
    // Model matrix: transformations are applied right-to-left order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Color the Object
    //glProgramUniform4f(gProgramId, objectColorLoc, 0.5f, 0.0f, 0.5f, 1.0f);
// Color the bottom and top with texture
    glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		// bottom
    glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		// top
    // unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // Set color to black for the sides
    glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 0.0f, 0.0f, 0.0f, 1.0f);
    // Draw the sides
    glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	// sides
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}



// Function to create a texture from a provided image file and return the success status.
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    // Variables to store width, height, and channel info of the image
    int width, height, nrChannels;

    // Flip texture vertically during loading as OpenGL expects the first pixel 
    // in the bottom left, while images usually have the first pixel in the top left
    stbi_set_flip_vertically_on_load(true);

    // Load the texture data from the provided file
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    // Check if the texture data has been loaded successfully
    if (!data)
    {
        std::cerr << "Error: Unable to load texture: " << filename << std::endl;
        return false;
    }

    // Generate a new texture ID
    glGenTextures(1, &textureId);

    // Bind the texture to a texture target
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Define the texture image, set color format based on the number of channels
    glTexImage2D(GL_TEXTURE_2D, 0, nrChannels == 3 ? GL_RGB : GL_RGBA, width, height, 0,
        nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Set texture wrapping and filtering modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture object from the current OpenGL texture target
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free the image data, as it's now on the GPU
    stbi_image_free(data);

    return true;
}


void UDestroyTexture(GLuint textureId)
{
    glDeleteTextures(1, &textureId);
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, &mesh.vbo);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}



void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
