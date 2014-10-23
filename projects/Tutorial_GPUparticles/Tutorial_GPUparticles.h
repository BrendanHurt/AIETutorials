#ifndef _TUTORIAL_GPUPARTICLES_H_
#define _TUTORIAL_GPUPARTICLES_H_

#pragma once

#include "Application.h"
//#include <glm/glm.hpp>
#include "GPUParticle.h"

class Tutorial_GPUparticles : public Application
{
public:

	Tutorial_GPUparticles();
	virtual ~Tutorial_GPUparticles();

	GPUParticle * m_Emitter;
protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};
#endif