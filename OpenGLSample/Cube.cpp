#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cube.h"



void UCreateMeshCube(GLMesh& mesh)
{
	// Position and Color data
	GLfloat verts[] = {
		//Positions				//Normals
		// ------------------------------------------------------

		 // Front face (Red)
		 -0.375f,  0.0f,   0.1875f,  1.0f, 0.0f, 0.0f,
		  0.375f,  0.0f,   0.1875f,  1.0f, 0.0f, 0.0f,
		  0.375f,  0.375f, 0.1875f,  1.0f, 0.0f, 0.0f,
		 -0.375f,  0.375f, 0.1875f,  1.0f, 0.0f, 0.0f,

		 // Back face (Green)
		 -0.375f,  0.0f,  -0.1875f,  0.0f, 1.0f, 0.0f,
		  0.375f,  0.0f,  -0.1875f,  0.0f, 1.0f, 0.0f,
		  0.375f,  0.375f,-0.1875f,  0.0f, 1.0f, 0.0f,
		 -0.375f,  0.375f,-0.1875f,  0.0f, 1.0f, 0.0f,

		 // Top face (Blue)
		 -0.375f,  0.375f, 0.1875f,  0.0f, 0.0f, 1.0f,
		  0.375f,  0.375f, 0.1875f,  0.0f, 0.0f, 1.0f,
		  0.375f,  0.375f,-0.1875f,  0.0f, 0.0f, 1.0f,
		 -0.375f,  0.375f,-0.1875f,  0.0f, 0.0f, 1.0f,

		 // Bottom face (Yellow)
		 -0.375f,  0.0f,   0.1875f,  1.0f, 1.0f, 0.0f,
		  0.375f,  0.0f,   0.1875f,  1.0f, 1.0f, 0.0f,
		  0.375f,  0.0f,  -0.1875f,  1.0f, 1.0f, 0.0f,
		 -0.375f,  0.0f,  -0.1875f,  1.0f, 1.0f, 0.0f,

		 // Left face (Cyan)
		 -0.375f,  0.0f,  -0.1875f,  0.0f, 1.0f, 1.0f,
		 -0.375f,  0.0f,   0.1875f,  0.0f, 1.0f, 1.0f,
		 -0.375f,  0.375f, 0.1875f,  0.0f, 1.0f, 1.0f,
		 -0.375f,  0.375f,-0.1875f,  0.0f, 1.0f, 1.0f,

		 // Right face (Magenta)
		  0.375f,  0.0f,  -0.1875f,  1.0f, 0.0f, 1.0f,
		  0.375f,  0.0f,   0.1875f,  1.0f, 0.0f, 1.0f,
		  0.375f,  0.375f, 0.1875f,  1.0f, 0.0f, 1.0f,
		  0.375f,  0.375f,-0.1875f,  1.0f, 0.0f, 1.0f
	};

	// Index data
	GLuint indices[] = {
		/* Plane triangles
		0, 1, 2,
		3, 4, 5,*/

		// Front face
		6, 7, 8, 6, 8, 9,

		// Back face
		10, 11, 12, 10, 12, 13,

		// Top face
		14, 15, 16, 14, 16, 17,

		// Bottom face
		18, 19, 20, 18, 20, 21,

		// Left face
		22, 23, 24, 22, 24, 25,

		// Right face
		26, 27, 28, 26, 28, 29
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create 2 buffers: first one for the vertex data; second one for the indices
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); // Activates the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
};