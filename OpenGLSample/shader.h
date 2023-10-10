#pragma once


/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Kelly Perez CS-330 FInal Project"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handle for the vertex buffer object
        GLuint nVertices;    // Number of indices of the mesh
        GLuint nIndices;
    };



    // Main GLFW window
    GLFWwindow* gWindow = nullptr;


    // Shader program
    GLuint gSurfaceProgramId;
    GLuint gLightProgramId;

    //Shape Meshes from Professor Brian
    Meshes meshes;
    GLuint gMonitor;
    GLuint gDesk;
    GLMesh gMesh;
    GLuint gCube;
    GLuint gCoffee;
    GLuint gCoffee2;
    GLuint gDeskpad;
    GLuint gKeyboard;
    GLuint gMouse;
    GLuint gSpeaker;
    GLuint gSpeakerBox;
    GLuint gFrame;

    GLuint gTextureId;
    glm::vec2 gUVScale(1.0f, 1.0f);
    GLint gTexWrapMode = GL_REPEAT;


    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 7.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Subject position and scale
    glm::vec3 gCubePosition(0.0f, 10.5f, 10.0f);
    glm::vec3 gCubeScale(0.3f);

    // Cube and light color
    glm::vec3 gObjectColor(0.f, 1.0f, 0.0f);
    glm::vec3 gLightColor(0.0f, 0.0f, 0.0f);

    // Cube2 and light color
    glm::vec3 gObjectColor2(1.f, 1.0f, 1.0f);
    glm::vec3 gLightColor2(0.0f, 1.0f, 0.0f);

    // Light position and scale
    glm::vec3 gLightPosition(5.5f, 10.5f, 10.0f);
    glm::vec3 gLightScale(0.3f);

    // Light2 position and scale
    glm::vec3 gLightPosition2(-5.0f, 10.5f, 10.0f);
    glm::vec3 gLightScale2(0.3f); 

    // Lamp animation
    bool gIsLampOrbiting = false;
}

////////// Professor Brian Battersby's Shader and lighting code //////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
/* Surface Vertex Shader Source Code*/
const GLchar* surfaceVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 vertexPosition; // VAP position 0 for vertex position data
layout(location = 1) in vec3 vertexNormal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexFragmentNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(vertexPosition, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexFragmentNormal = mat3(transpose(inverse(model))) * vertexNormal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Surface Fragment Shader Source Code*/
const GLchar* surfaceFragmentShaderSource = GLSL(440,

    in vec3 vertexFragmentNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec4 objectColor;
uniform vec3 ambientColor;
uniform vec3 light1Color;
uniform vec3 light1Position;
uniform vec3 light2Color;
uniform vec3 light2Position;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;
uniform bool ubHasTexture;
uniform float ambientStrength = 0.1f; // Set ambient or global lighting strength
uniform float specularIntensity1 = 0.8f;
uniform float highlightSize1 = 16.0f;
uniform float specularIntensity2 = 0.8f;
uniform float highlightSize2 = 16.0f;
uniform vec4 fillLightColor;
uniform vec3 fillLightPos;


void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting
    vec3 ambient = ambientStrength * ambientColor; // Generate ambient light color

    // Apply Intensity to Key and Fill lights
    float keyLightIntensity;
    float fillLightIntensity;
    vec3 keyLight = keyLightIntensity * vec3(light1Color);
    vec3 fillLight = fillLightIntensity * vec3(light2Color);

    //**Calculate Diffuse lighting**
    vec3 norm = normalize(vertexFragmentNormal); // Normalize vectors to 1 unit
    vec3 light1Direction = normalize(light1Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact1 = max(dot(norm, light1Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse1 = impact1 * light1Color; // Generate diffuse light color
    vec3 light2Direction = normalize(light2Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact2 = max(dot(norm, light2Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse2 = impact2 * light2Color; // Generate diffuse light color

    //**Calculate Specular lighting**
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir1 = reflect(-light1Direction, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent1 = pow(max(dot(viewDir, reflectDir1), 0.0), highlightSize1);
    vec3 specular1 = specularIntensity1 * specularComponent1 * light1Color;
    vec3 reflectDir2 = reflect(-light2Direction, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
    vec3 specular2 = specularIntensity2 * specularComponent2 * light2Color;

    //**Calculate phong result**
    //Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);
    vec3 phong1;
    vec3 phong2;

    if (ubHasTexture == true)
    {
        phong1 = (ambient + diffuse1 + specular1) * textureColor.xyz;
        phong2 = (ambient + diffuse2 + specular2) * textureColor.xyz;
    }
    else
    {
        phong1 = (ambient + diffuse1 + specular1) * objectColor.xyz;
        phong2 = (ambient + diffuse2 + specular2) * objectColor.xyz;
    }

    fragmentColor = vec4(phong1 + phong2, 1.0); // Send lighting results to GPU
    //fragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*  Light Object Shader Source Code*/
const GLchar* lightVertexShaderSource = GLSL(330,
    layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Light Object Shader Source Code*/
const GLchar* lightFragmentShaderSource = GLSL(330,
    out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0); // set all 4 vector values to 1.0
}
);
/////////////////////////////////////////////////////////////////////////////////////////////////////////