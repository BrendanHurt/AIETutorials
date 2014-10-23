#ifndef __Tutorial1_H_
#define __Tutorial1_H_

#include "Application.h"
#include <glm/glm.hpp>

// derived application class that wraps up all globals neatly
class Tutorial1 : public Application
{
public:

	Tutorial1();
	virtual ~Tutorial1();

	struct ObjectData
	{
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec4 FillColor;
		unsigned int Segments;
		float InnerRadius, OuterRadius;
	};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};
#endif