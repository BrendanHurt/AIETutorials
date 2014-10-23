#include "Tutorial_Lighting.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

DirectionalLight Lighting;

Tutorial_Lighting::Tutorial_Lighting()
{

}

Tutorial_Lighting::~Tutorial_Lighting()
{

}

void Tutorial_Lighting::CreateOpenGLBuffers(FBXFile *a_FBX)
{
	//create the buffers and whatnot data for the meshes
	for (unsigned int i = 0; i < a_FBX->getMeshCount(); ++i)
	{
		FBXMeshNode* Mesh = a_FBX->getMeshByIndex(i);

		//storage for the opengl data in 3 unsigned ints
		unsigned int* uiData = new unsigned int[3];

		glGenVertexArrays(1, &uiData[0]);
		glBindVertexArray(uiData[0]);

		glGenBuffers(1, &uiData[1]);
		glGenBuffers(1, &uiData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, uiData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiData[2]);

		glBufferData(GL_ARRAY_BUFFER, Mesh->m_vertices.size() * sizeof(FBXVertex), Mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh->m_indices.size() * sizeof(unsigned int), Mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char *)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		Mesh->m_userData = uiData;
	}
}

void Tutorial_Lighting::CleanOpenGLBuffers(FBXFile* a_FBX)
{
	//bind the vertex array and draw the mesh
	for (unsigned int i = 0; i < a_FBX->getMeshCount(); i++)
	{
		FBXMeshNode* Mesh = a_FBX->getMeshByIndex(i);

		unsigned int* uiData = (unsigned int*)Mesh->m_userData;

		glDeleteVertexArrays(1, &uiData[0]);
		glDeleteBuffers(1, &uiData[1]);
		glDeleteBuffers(1, &uiData[2]);

		delete[] uiData;
	}
}

bool Tutorial_Lighting::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	Lighting.AmbientLight = glm::vec3(0.1, 0.1, 0.7);
	Lighting.LightDirection = glm::vec3(0, 0, -1);
	Lighting.LightColor = glm::vec3(0, 1, 1);
	Lighting.SpecularLight = glm::vec3(1, 1, 0);

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	m_VertShader = Utility::loadShader("./Resources/Shaders/Light.vert", GL_VERTEX_SHADER);
	m_FragShader = Utility::loadShader("./Resources/Shaders/Light.frag", GL_FRAGMENT_SHADER);

	const char * cInputs[] = { "Position", "Normal" };

	m_Shader = Utility::createProgram(m_VertShader, 0, 0, 0, m_FragShader, 2, cInputs);

	m_FBX = new FBXFile();
	m_FBX->load("../../bin/stanford/Bunny.fbx");

	CreateOpenGLBuffers(m_FBX);

	//create a perspective with a 90 degree FOV and wide screen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Tutorial_Lighting::onUpdate(float a_deltaTime) 
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
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Tutorial_Lighting::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));

	//RenderFBXSceneResource(m_pFBX, viewMatrix, m_projectionMatrix);

	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	glUseProgram(m_Shader);

	//fetch location of the view and projection matricies
	unsigned int Location = glGetUniformLocation(m_Shader, "view");
	glUniformMatrix4fv(Location, 1, false, glm::value_ptr(viewMatrix));

	Location = glGetUniformLocation(m_Shader, "projection");
	glUniformMatrix4fv(Location, 1, false, glm::value_ptr(m_projectionMatrix));

	//send the position of the camera
	Location = glGetUniformLocation(m_Shader, "cameraPosition");
	glUniform3fv(Location, 1, glm::value_ptr(m_cameraMatrix[3]));

	Location = glGetUniformLocation(m_Shader, "LightAmbient");
	glUniform3fv(Location, 1, glm::value_ptr(Lighting.AmbientLight));

	Location = glGetUniformLocation(m_Shader, "LightDirection");
	glUniform3fv(Location, 1, glm::value_ptr(Lighting.LightDirection));

	Location = glGetUniformLocation(m_Shader, "LightColor");
	glUniform3fv(Location, 1, glm::value_ptr(Lighting.LightColor));

	Location = glGetUniformLocation(m_Shader, "LightSpecular");
	glUniform3fv(Location, 1, glm::value_ptr(Lighting.SpecularLight));

	//bind the vertex array and draw the mesh
	for (unsigned int i = 0; i < m_FBX->getMeshCount(); i++)
	{
		FBXMeshNode* Mesh = m_FBX->getMeshByIndex(i);

		unsigned int* uiData = (unsigned int*)Mesh->m_userData;

		glBindVertexArray(uiData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)Mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void Tutorial_Lighting::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();

	CleanOpenGLBuffers(m_FBX);

	glDeleteShader(m_Shader);
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Tutorial_Lighting();
	
	if (app->create("AIE - Tutorial_Lighting",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}