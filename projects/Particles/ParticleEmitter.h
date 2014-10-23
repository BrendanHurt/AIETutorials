#pragma once
#ifndef _PARTICLEEMITTER_H_
#define _PARTICLEEMITTER_H_

#include "Particle.h"
#include <GL/glew.h>
#include <list>

class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void Initialise(unsigned int a_MaxParticles, unsigned int a_EmitRate, float a_LifeTimeMin, float a_LifeTimeMax, float a_VelocityMin, 
		float a_VelocityMax, float a_StartSize, float a_EndSize, const glm::vec4&a_StartColor, const glm::vec4& a_EndColor);

	void Emit();

	void update(float a_DeltaTime, const glm::mat4& a_CameraTransform);

	void Draw();

protected:
	std::list<Particle*> m_AliveParticles;
	std::list<Particle*> m_DeadParticles;
	Particle* m_Particles;

	unsigned int m_VBO, m_IBO, m_VAO;
	ParticleVertex* m_VertexData;
	unsigned int* m_IndexData;

	glm::vec3 m_Position;

	float m_EmitTimer;
	float m_EmitRate;

	float m_LifeSpanMin;
	float m_LifeSpanMax;

	float m_VelocityMin;
	float m_VelocityMax;

	float m_StartSize;
	float m_EndSize;

	glm::vec4 m_StartColor;
	glm::vec4 m_EndColor;
};
#endif