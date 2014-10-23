#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>

// derived application class that wraps up all globals neatly
class Tutorial6 : public Application
{
public:

	Tutorial6();
	virtual ~Tutorial6();

	// load shaders and link shader program
	unsigned int m_vertShader;
	unsigned int m_fragShader;
	unsigned int m_shader;
	FBXFile *m_fbx;

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	virtual void createOpenGLBuffers(FBXFile* a_fbx);
	virtual void cleanupOpenGLBuffers(FBXFile* a_fbx);

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

struct DLight{

	glm::vec3 AmbientLight;
	glm::vec3 LightDirection;
	glm::vec3 LightColor;
	glm::vec3 SpecularLight;

};