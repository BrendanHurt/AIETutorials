#pragma once

#include "Application.h"
#include <glm/glm.hpp>

struct Vertex4p4c
{
	glm::vec4 Position;
	glm::vec4 Color;
};

// derived application class that wraps up all globals neatly
class Tutorial2 : public Application
{
public:

	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_VAO;
	unsigned int m_ProgramID;

	Tutorial2();
	virtual ~Tutorial2();

protected:

	void GenerateGrid(unsigned int uiRows, unsigned int uiCollums);

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};