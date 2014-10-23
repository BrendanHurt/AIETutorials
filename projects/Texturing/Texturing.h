#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <stb_image.h>

// derived application class that wraps up all globals neatly
class Texturing : public Application
{
public:

	Texturing();
	virtual ~Texturing();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	void LoadTexture(char * FilePath);

	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_VAO;

	unsigned int m_Shader;
	unsigned int m_Texture;
	unsigned int m_VertShader;
	unsigned int m_FragShader;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};