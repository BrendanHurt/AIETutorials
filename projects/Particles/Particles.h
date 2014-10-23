#pragma once
#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "Application.h"
#include <glm/glm.hpp>

#include "ParticleEmitter.h"
// derived application class that wraps up all globals neatly
class Particles : public Application
{
public:

	Particles();
	virtual ~Particles();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	unsigned int m_Program;

	ParticleEmitter* m_Emitter;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};
#endif