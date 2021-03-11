// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID1 = LoadShaders( "VertexShader1.vertexshader", "FragmentShader1.fragmentshader" );
	GLuint programID2 = LoadShaders("VertexShader2.vertexshader", "FragmentShader2.fragmentshader");

	GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
	GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");

	static const GLfloat triang_buffer1[] = { 
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};

	static const GLfloat triang_buffer2[] = {
		-0.2f, -0.5f, 0.0f,
		 0.8f, -0.5f, 0.0f,
		 0.3f,  0.5f, 0.0f,
	};

	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triang_buffer1), triang_buffer1, GL_STATIC_DRAW);
	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triang_buffer2), triang_buffer2, GL_STATIC_DRAW);

	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(90.0f), 
		4.0f / 3.0f,       
		0.1f,              
		100.0f             
	);

	glm::vec3 camera = glm::vec3(0, 0, 4);

	glm::mat4 Model = glm::mat4(1.0f);

	float angle = 0.0;

	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glm::mat4 View = glm::lookAt(
			camera,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		glm::mat4 MVP = projectionMatrix * View * Model;

		angle += glm::radians(0.3f);

		camera = glm::vec3(4 * glm::cos(angle), 0, 4 * glm::sin(angle));

		// Use our shader
		glUseProgram(programID1);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);

		glUseProgram(programID2);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID1);
	glDeleteBuffers(1, &vertexbuffer2);
	//glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID2);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

