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
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1024, 768, "Pyramide", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	glm::vec3 camera = glm::vec3(0, 0, 6);
	glm::mat4 View = glm::lookAt(
								camera, 
								glm::vec3(0,0,0), 
								glm::vec3(0,1,0)  
						   );
	glm::mat4 Model = glm::mat4(1.0f);
	
	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, 0.0f, 1.0f, //1
		 0.0f, 2.0f, 0.0f, //2
		 1.0f, 0.0f, 1.0f, //3
		-1.0f, 0.0f, 1.0f, //1
		 0.0f, 2.0f, 0.0f, //2
		-1.0f, 0.0f,-1.0f, //4
		 0.0f, 2.0f, 0.0f, //2
		-1.0f, 0.0f,-1.0f, //4 
		 1.0f, 0.0f,-1.0f, //5
		 1.0f, 0.0f,-1.0f, //5
		 0.0f, 2.0f, 0.0f, //2
		 1.0f, 0.0f, 1.0f, //3
		-1.0f, 0.0f, 1.0f, //1
		 1.0f, 0.0f, 1.0f, //3
		 1.0f, 0.0f,-1.0f, //5
		-1.0f, 0.0f, 1.0f, //1
		 1.0f, 0.0f,-1.0f, //5
		-1.0f, 0.0f,-1.0f, //4
	};

	static const GLfloat g_color_buffer_data[] = { 
		0.2f,  1.0f,  0.3f, //1
		1.0f,  1.0f,  1.0f, //2
		1.0f,  0.2f,  0.3f, //3
		0.2f,  1.0f,  0.3f, //1
		1.0f,  1.0f,  1.0f, //2
		0.2f,  0.3f,  1.0f, //4
		1.0f,  1.0f,  1.0f, //2
		0.2f,  0.3f,  1.0f, //4
		0.5f,  0.5f,  0.5f, //5
		0.5f,  0.5f,  0.5f, //5
		1.0f,  1.0f,  1.0f, //2
		1.0f,  0.2f,  0.3f, //3
		0.2f,  1.0f,  0.3f, //1
		1.0f,  0.2f,  0.3f, //3
		0.5f,  0.5f,  0.5f, //5
		0.2f,  1.0f,  0.3f, //1
		0.5f,  0.5f,  0.5f, //5
		0.2f,  0.3f,  1.0f  //4
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	float angle = 0.0;

	do{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 View = glm::lookAt(
			camera,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		glm::mat4 MVP = Projection * View * Model;

		angle += glm::radians(0.03f);

		camera = glm::vec3(4 * glm::cos(angle), 0, 4 * glm::sin(angle));

		glUseProgram(programID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  
			3,                  
			GL_FLOAT,           
			GL_FALSE,           
			0,                  
			(void*)0           
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                
			3,                                
			GL_FLOAT,                         
			GL_FALSE,                         
			0,                                
			(void*)0                          
		);

		glDrawArrays(GL_TRIANGLES, 0, 18);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} 
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();

	return 0;
}

