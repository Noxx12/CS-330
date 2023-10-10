#ifndef CUBE_H
#define CUBE_H

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct GLMesh {
    GLuint vao;
    GLuint vbos[2]; // We have two VBOs: one for vertex data, one for indices
    GLsizei nVertices; // Number of vertices
    GLsizei nIndices; // Number of indices
};

void UCreateMesh(GLMesh& mesh);

#endif // CUBE_H
