#include "Tutorial2.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Tutorial2::Tutorial2()
{

}

Tutorial2::~Tutorial2()
{

}

void Tutorial2::GenerateGrid(unsigned int uiRows, unsigned int uiCollums)
{
	Vertex4p4c* aoVerticies = new Vertex4p4c[uiRows * uiCollums];

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	//create and bind a Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (uiRows * uiCollums) * sizeof(Vertex4p4c), aoVerticies, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex4p4c), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex4p4c), (void *)(4 * sizeof(float)));

	//assign color and position variables to aoVerticies
	for (unsigned int R = 0; R < uiRows; R++)
	{
		for (unsigned int C = 0; C < uiCollums; C++)
		{
			aoVerticies[R * uiCollums + C].Position = glm::vec4((float)C, 0, (float)R, 1);
			glm::vec3 v3Color = glm::vec3(sinf(C / (float)(uiCollums)* (R / (float)(uiRows - 1))));
			aoVerticies[R * uiCollums + C].Color = glm::vec4(v3Color, 1.0f);
		}
	}

	//defining index based on quad count
	unsigned int* uiIndicies = new unsigned int[(uiRows - 1) * (uiCollums - 1) * 6];

	//create an array of indicies
	unsigned int Index = 0;
	for (unsigned int R = 0; R < (uiRows - 1); R++)
	{
		for (unsigned int C = 0; C < (uiCollums - 1); C++)
		{
			//trianle 1
			uiIndicies[Index++] = (R + 1) * uiCollums + (C + 1);
			uiIndicies[Index++] = (R + 1) * uiCollums + C;
			uiIndicies[Index++] = R * uiCollums + C;

			//traingle 2
			uiIndicies[Index++] = R * uiCollums + (C + 1);
			uiIndicies[Index++] = (R + 1) * uiCollums + (C + 1);
			uiIndicies[Index++] = R * uiCollums + C;
		}
	}

	//create and bind a index buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (uiRows - 1) * (uiCollums - 1) * 6 * sizeof(unsigned int), uiIndicies, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//create and bind a vertex array
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (uiRows * uiCollums) * sizeof(Vertex4p4c), aoVerticies, (4 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex4p4c), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex4p4c), (void *)(4 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (uiRows - 1) * (uiCollums - 1) * 6 * sizeof(unsigned int), uiIndicies, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVerticies;
}

bool Tutorial2::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	GenerateGrid(100, 100);

	const char* VertSource = "#version 150\n \
							 in vec4 Position; \
							 out vec4 Color; \
							 out vec4 vColor; \
							 uniform mat4 ProjectionView; \
							 void main () { vColor = Color; gl_Position = ProjectionView * Position; }";
	const char* FragSource = "#version 150\n \
							 in vec4 vColor; \
							 out vec4 FragColor; \
							 void main() { FragColor = vColor; }";

	int iSuccess = GL_FALSE;

	unsigned int uiVertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int uiFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(uiVertexShader, 1, (const char**)&VertSource, 0);
	glCompileShader(uiVertexShader);

	glGetShaderiv(uiVertexShader, GL_COMPILE_STATUS, &iSuccess);
	if (iSuccess == GL_FALSE)
	{
		int InfoLogLength = 0;
		glGetShaderiv(uiVertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
		char* InfoLog = new char[InfoLogLength];

		glGetShaderInfoLog(uiVertexShader, InfoLogLength, 0, InfoLog);
		printf("Error: Failed to compile vertex shader!\n");
		printf("%s", InfoLog);
		printf("\n");
		delete[] InfoLog;
	}

	glShaderSource(uiFragmentShader, 1, (const char**)&FragSource, 0);
	glCompileShader(uiFragmentShader);

	glGetShaderiv(uiFragmentShader, GL_COMPILE_STATUS, &iSuccess);
	if (iSuccess == GL_FALSE)
	{
		int iInfoLogLength = 0;
		glGetShaderiv(uiFragmentShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		char* cInfoLog = new char[iInfoLogLength];

		glGetShaderInfoLog(uiFragmentShader, iInfoLogLength, 0, cInfoLog);
		printf("Error: Failed to compile fragment shader!\n");
		printf("%s", cInfoLog);
		printf("\n");
		delete[] cInfoLog;
	}

	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID, uiVertexShader);
	glAttachShader(m_ProgramID, uiFragmentShader);
	glBindAttribLocation(m_ProgramID, 0, "Position");
	glBindAttribLocation(m_ProgramID, 1, "Colour");
	glLinkProgram(m_ProgramID);

	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &iSuccess);
	if (iSuccess == GL_FALSE)
	{
		int iInfoLogLength = 0;
		glGetShaderiv(m_ProgramID, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		char* cInfoLog = new char[iInfoLogLength];

		glGetShaderInfoLog(m_ProgramID, iInfoLogLength, 0, cInfoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s", cInfoLog);
		printf("\n");
		delete[] cInfoLog;
	}

	glDeleteShader(uiFragmentShader);
	glDeleteShader(uiVertexShader);

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	return true;
}

void Tutorial2::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Tutorial2::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_ProgramID);
	unsigned int uiProjectionViewUniform = glGetUniformLocation(m_ProgramID, "ProjectionView");
	glUniformMatrix4fv(uiProjectionViewUniform, 1, false, glm::value_ptr(m_projectionMatrix));

	glBindVertexArray(m_VAO);
	int iRows = 10;
	int iCollums = 10;
	unsigned int uiIndexCount = (iRows - 1) * (iCollums - 1) * 6;
	glDrawElements(GL_TRIANGLES, uiIndexCount, GL_UNSIGNED_INT, 0);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));
}

void Tutorial2::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Tutorial2();
	
	if (app->create("AIE - Tutorial2",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}