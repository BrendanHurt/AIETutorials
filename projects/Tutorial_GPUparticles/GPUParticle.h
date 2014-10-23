#ifndef _GPUPARTICLE_H_
#define _GPUPARTICLE_H_

#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

struct ParticleGPU
{
	ParticleGPU() : lifetime(1), lifespan(0) {}

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 lifetime;
	glm::vec3 lifespan;
};

class GPUParticle {
public:
	GPUParticle();

	virtual ~GPUParticle();

	void Initialise(unsigned int a_MaxParticles,
	float a_LifeSpanMin,
	float a_LifeSpanMax,
	float a_VelocityMin,
	float a_VelocityMax,
	float a_StartSize,
	float a_EndSize,
	const glm::vec4& a_StartColor,
	const glm::vec4& a_EndColor);

	void Draw(const glm::mat4& a_CameraTransform, const glm::mat4& a_Projection);

	void SetPosition(const glm::vec3 a_Position) { m_Position = a_Position; }

protected:

	ParticleGPU* m_Particles;

	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();

	unsigned int m_MaxParticles;

	glm::vec3 m_Position;

	float m_LifeSpanMin;
	float m_LifeSpanMax;

	float m_VelocityMin;
	float m_VelocityMax;

	float m_StartSize;
	float m_EndSize;

	glm::vec4 m_StartColor;
	glm::vec4 m_EndColor;

	unsigned int m_ActiveBuffer;
	unsigned int m_VAO[2];
	unsigned int m_VBO[2];

	unsigned int m_DrawShader;
	unsigned int m_UpdateShader;
};
#endif