#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include "GPUParticleSystem.h"

// derived application class that wraps up all globals neatly
class Tutorial_AIE_GPUParticles : public Application
{
public:

	Tutorial_AIE_GPUParticles();
	virtual ~Tutorial_AIE_GPUParticles();

protected:

	GPUParticleEmitter* m_Emitter;

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};