#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter()
: m_Particles(nullptr),
m_Position(0, 0, 0),
m_VAO(0), m_VBO(0), m_IBO(0),
m_VertexData(nullptr), m_IndexData(nullptr)
{

}

ParticleEmitter::~ParticleEmitter()
{
	delete[] m_Particles;
	delete[] m_IndexData;
	delete[] m_VertexData;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
}

void ParticleEmitter::Initialise(unsigned int a_MaxParticles, 
								unsigned int a_EmitRate, 
								float a_LifeTimeMin, 
								float a_lifeTimeMax,
								float a_VelocityMin, 
								float a_VelocityMax, 
								float a_StartSize, 
								float a_EndSize, 
								const glm::vec4& a_StartColor,
								const glm::vec4& a_EndColor)
{
	//set up timers
	m_EmitTimer = 0;
	m_EmitRate = 1.0f / a_EmitRate;

	//store all variables that have been passed in
	m_StartColor = a_StartColor;
	m_EndColor = a_EndColor;

	m_StartSize = a_StartSize;
	m_EndSize = a_EndSize;

	m_VelocityMin = a_VelocityMin;
	m_VelocityMax = a_VelocityMax;

	m_LifeSpanMin = a_LifeTimeMin;
	m_LifeSpanMax = a_lifeTimeMax;

	//create particles array and store it in the dead list
	m_Particles = new Particle[a_MaxParticles];
	for (unsigned int i = 0; i < a_MaxParticles; ++i)
	{
		m_DeadParticles.push_back(&m_Particles[i]);
	}

	//create the array of verticies for the particles
	//4 particles per quad
	m_VertexData = new ParticleVertex[a_MaxParticles * 4];

	//create index buffer data for the particles
	//6 indicies per quad
	m_IndexData = new unsigned int[a_MaxParticles * 6];
	for (unsigned int i = 0; i < a_MaxParticles; ++i)
	{
		m_IndexData[i * 6 + 0] = i * 4 + 0;
		m_IndexData[i * 6 + 1] = i * 4 + 1;
		m_IndexData[i * 6 + 2] = i * 4 + 2;

		m_IndexData[i * 6 + 3] = i * 4 + 0;
		m_IndexData[i * 6 + 4] = i * 4 + 2;
		m_IndexData[i * 6 + 5] = i * 4 + 3;
	}

	//create opengl buffer
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, a_MaxParticles * 4 * sizeof(ParticleVertex), m_VertexData, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_MaxParticles * 6 * sizeof(unsigned int), m_IndexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //color

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + sizeof(glm::vec4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ParticleEmitter::Emit()
{
	//only emit a particle if theres a dead particle available
	if (m_DeadParticles.empty())
	{
		return;
	}

	//remove a newly spawened from the dead list
	Particle* SelectedParticle = m_DeadParticles.back();
	m_DeadParticles.pop_back();

	//assing the starting postition for the particle
	SelectedParticle->position = m_Position;

	//randomise the lifespan of the the particle
	SelectedParticle->lifetime = 0;
	SelectedParticle->lifespan = (rand() / (float)RAND_MAX) * (m_LifeSpanMax - m_LifeSpanMin) + m_LifeSpanMin;

	//randomise the velocity and direction of the particle
	float fVelocity = (rand() / (float)RAND_MAX) * (m_VelocityMax - m_VelocityMin) + m_VelocityMin;
	SelectedParticle->velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	SelectedParticle->velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	SelectedParticle->velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;
	SelectedParticle->velocity = glm::normalize(SelectedParticle->velocity) * fVelocity;

	//add the particle to the alive list
	m_AliveParticles.push_back(SelectedParticle);
}

void ParticleEmitter::update(float a_DeltaTime, const glm::mat4& a_CameraTransform)
{
	//spawn the particles
	m_EmitTimer += a_DeltaTime;
	while (m_EmitTimer > m_EmitRate)
	{
		Emit();
		m_EmitTimer -= m_EmitRate;
	}

	auto iter = m_AliveParticles.begin();
	unsigned int Quad = 0;

	//update particles and billboard live particles
	while (iter != m_AliveParticles.end())
	{
		Particle* SelectedParticle = (*iter);

		SelectedParticle->lifetime += a_DeltaTime;
		if (SelectedParticle->lifetime >= SelectedParticle->lifespan)
		{
			//send particles to the dead list
			m_DeadParticles.push_back(SelectedParticle);
			iter = m_AliveParticles.erase(iter);
		}
		else
		{
			//move the live particles
			SelectedParticle->position += SelectedParticle->velocity * a_DeltaTime;

			//particle size
			SelectedParticle->size = glm::mix(m_StartSize, m_EndSize, SelectedParticle->lifetime / SelectedParticle->lifespan);

			//particle color
			SelectedParticle->colour = glm::mix(m_StartColor, m_EndColor, SelectedParticle->lifetime / SelectedParticle->lifespan);

			//make the quad the correct size
			float fHalfSize = SelectedParticle->size * 0.5f;

			m_VertexData[Quad * 4 + 0].position = glm::vec4(fHalfSize, fHalfSize, 0, 1);
			m_VertexData[Quad * 4 + 0].colour = SelectedParticle->colour;

			m_VertexData[Quad * 4 + 1].position = glm::vec4(-fHalfSize, fHalfSize, 0, 1);
			m_VertexData[Quad * 4 + 1].colour = SelectedParticle->colour;

			m_VertexData[Quad * 4 + 2].position = glm::vec4(-fHalfSize, -fHalfSize, 0, 1);
			m_VertexData[Quad * 4 + 2].colour = SelectedParticle->colour;

			m_VertexData[Quad * 4 + 3].position = glm::vec4(fHalfSize, -fHalfSize, 0, 1);
			m_VertexData[Quad * 4 + 3].colour = SelectedParticle->colour;

			glm::vec3 zAxis = glm::normalize(a_CameraTransform[3].xyz - SelectedParticle->position);
			glm::vec3 xAxis = glm::cross(glm::vec3(a_CameraTransform[1].xyz), zAxis);
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);
			glm::mat3 Billboard(xAxis, yAxis, zAxis);

			m_VertexData[Quad * 4 + 0].position.xyz = Billboard * m_VertexData[Quad * 4 + 0].position.xyz + SelectedParticle->position;
			m_VertexData[Quad * 4 + 1].position.xyz = Billboard * m_VertexData[Quad * 4 + 1].position.xyz + SelectedParticle->position;
			m_VertexData[Quad * 4 + 2].position.xyz = Billboard * m_VertexData[Quad * 4 + 2].position.xyz + SelectedParticle->position;
			m_VertexData[Quad * 4 + 3].position.xyz = Billboard * m_VertexData[Quad * 4 + 3].position.xyz + SelectedParticle->position;

			++Quad;
			++iter;
		}
	}
}

void ParticleEmitter::Draw()
{
	//snyc the particle vertex buffer based on the number of live particles
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_AliveParticles.size() * 4 * sizeof(ParticleVertex), m_VertexData);

	//draw particles
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)m_AliveParticles.size() * 6, GL_UNSIGNED_INT, 0);
}