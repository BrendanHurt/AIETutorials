#include "Tutorial3.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Tutorial3::Tutorial3()
{

}

Tutorial3::~Tutorial3()
{

}

void Tutorial3::InitFBXSceneResource(FBXFile *a_pScene)
{
	unsigned int uiMeshCount = a_pScene->getMeshCount();
	unsigned int uiMatCount = a_pScene->getMaterialCount();

	//loop through each mesh
	for (int i = 0; i < uiMeshCount; i++)
	{
		FBXMeshNode *pMesh = a_pScene->getMeshByIndex(i);

		OGL_FBXRenderData *RO = new OGL_FBXRenderData();
		pMesh->m_userData = RO;

		//generate VBO, IBO, VAO
		glGenBuffers(1, &RO->VBO);
		glGenBuffers(1, &RO->IBO);
		glGenVertexArrays(1, &RO->VAO);

		//bind VAO, then bind the VBO and IBO to the VAO
		glBindVertexArray(RO->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, RO->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RO->IBO);

		//send the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		//send the index data to the IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		//enable the attribute location that will be used on the sahders
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::ColourOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);

		glBindVertexArray(0);
	}
}

void Tutorial3::DestroyFBXSceneResource(FBXFile *a_pScene)
{
	unsigned int uiMeshCount = a_pScene->getMeshCount();
	unsigned int uiMatCount = a_pScene->getMaterialCount();

	//remove meshes
	for (unsigned int i = 0; i < uiMeshCount; i++)
	{
		FBXMeshNode* pMesh = a_pScene->getMeshByIndex(i);
		OGL_FBXRenderData *RO = (OGL_FBXRenderData *)pMesh->m_userData;

		//delete the buffers and fee memory from the graphics card
		glDeleteBuffers(1, &RO->VBO);
		glDeleteBuffers(1, &RO->IBO);
		glDeleteVertexArrays(1, &RO->VAO);

		delete RO;
	}

	/*for (int i = 0; i < uiMatCount; i++)
	{
		//get the current material
		FBXMaterial *pMaterial = a_pScene->getMaterialByIndex(i);
		for (int j = 0; j < FBXMaterial::TextureTypes_Count; j++)
		{
			if (pMaterial->textureIDs[j] != 0)
			{
				glDeleteTextures(1, &pMaterial->textureIDs[j]);
			}
		}
	}*/
}

void Tutorial3::UpdateFBXSceneResource(FBXFile * a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void Tutorial3::RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_View, glm::mat4 a_Projection)
{
	glUseProgram(m_Shader);

	//get the location of uniforms on the sahder
	GLint uDiffuseTexture = glGetUniformLocation(m_Shader, "DiffuseTexture");
	GLint uDiffuseColor = glGetUniformLocation(m_Shader, "DiffuseColor");

	GLint uModel = glGetUniformLocation(m_Shader, "Model");
	GLint uView = glGetUniformLocation(m_Shader, "View");
	GLint uProjection = glGetUniformLocation(m_Shader, "Projection");

	//for each mesh in the model
	for (int i = 0; i < a_pScene->getMeshCount(); i++)
	{
		//get the current mesh
		FBXMeshNode *pMesh = a_pScene->getMeshByIndex(i);

		//get the render data attached to the m_userData pointer for this mesh
		OGL_FBXRenderData *RO = (OGL_FBXRenderData *)pMesh->m_userData;

		//bind the texture to on the the active textures
		//bind each texture to a new active texture ID if you're using more than one texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pMesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

		//reset to the efault actuve texture
		glActiveTexture(GL_TEXTURE0);

		//tell the shader which texture to use
		glUniform1i(uDiffuseTexture, 1);

		//send the model, view and projection matricies to the shader
		glUniformMatrix4fv(uModel, 1, false, glm::value_ptr(pMesh->m_globalTransform));
		glUniformMatrix4fv(uView, 1, false, glm::value_ptr(a_View));
		glUniformMatrix4fv(uProjection, 1, false, glm::value_ptr(a_Projection));

		glBindVertexArray(RO->VAO);
		glDrawElements(GL_TRIANGLES, pMesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glUseProgram(0);
}

bool Tutorial3::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

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
	glEnable(GL_CULL_FACE);

	const char* acInputs[] = { "Position", "Color", "TexCoord1" };
	const char* acOutputs[] = { "outColor" };

	GLuint vShader = Utility::loadShader("./Shaders/SimpleFBX_Tutorial.vert", GL_VERTEX_SHADER);
	GLuint fShader = Utility::loadShader("./Shaders/SimpleFBX_Tutorial.frag", GL_FRAGMENT_SHADER);

	m_Shader = Utility::createProgram(vShader, 0, 0, 0, fShader, 3, acInputs, 1, acOutputs);

	//delete shaders to free up memory
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	m_pFBX = new FBXFile();

	if (!m_pFBX->load("./models_misc/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER))
	{
		printf("FBX file could not be loaded");
	}

	m_pFBX->initialiseOpenGLTextures();
	InitFBXSceneResource(m_pFBX);

	return true;
}

void Tutorial3::onUpdate(float a_deltaTime) 
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

	UpdateFBXSceneResource(m_pFBX);
}

void Tutorial3::onDraw() 
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

	RenderFBXSceneResource(m_pFBX, viewMatrix, m_projectionMatrix);
}

void Tutorial3::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();

	glDeleteShader(m_Shader);

	DestroyFBXSceneResource(m_pFBX);
	m_pFBX->unload();
	delete m_pFBX;
	m_pFBX = NULL;
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Tutorial3();
	
	if (app->create("AIE - Tutorial3",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}