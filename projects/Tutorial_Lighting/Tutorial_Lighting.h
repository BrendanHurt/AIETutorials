#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>

// derived application class that wraps up all globals neatly
class Tutorial_Lighting : public Application
{
public:

	Tutorial_Lighting();
	virtual ~Tutorial_Lighting();

protected:

	unsigned int m_VertShader;
	unsigned int m_FragShader;
	unsigned int m_Shader;

	FBXFile *m_FBX;

	void CreateOpenGLBuffers(FBXFile *a_FBX);
	void CleanOpenGLBuffers(FBXFile *a_FBX);

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	unsigned int VBO;
	unsigned int VAO;
	unsigned int IBO;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

struct DirectionalLight
{
	glm::vec3 AmbientLight;
	glm::vec3 LightDirection;
	glm::vec3 LightColor;
	glm::vec3 SpecularLight;
};