#include "GPUParticle.h"

GPUParticle::GPUParticle()
	: m_Particles(nullptr), m_MaxParticles(0),
	m_Position(0, 0, 0),
	m_DrawShader(0), m_UpdateShader(0)
{
	m_VAO[0] = 0;
	m_VAO[1] = 0;
	m_VBO[0] = 0;
	m_VBO[1] = 0;
}

GPUParticle::~GPUParticle()
{
	delete[] m_Particles;

	glDeleteVertexArrays(2, m_VAO);
	glDeleteBuffers(2, m_VBO);

	glDeleteProgram(m_DrawShader);
	glDeleteProgram(m_UpdateShader);
}

void GPUParticle::Initialise(unsigned int a_MaxParticles,
	float a_LifetimeMin, float a_LifetimeMax,
	float a_VelocityMin, float a_VelocityMax,
	float a_StartSize, float a_EndSize,
	const glm::vec4& a_StartColor, const glm::vec4& a_EndColor)
{
	m_MaxParticles = a_MaxParticles;

	m_StartColor = a_StartColor;
	m_EndColor = a_EndColor;

	m_StartSize = a_StartSize;
	m_EndSize = a_EndSize;

	m_VelocityMax = a_VelocityMax;
	m_VelocityMin = a_VelocityMin;

	m_LifeSpanMax = a_LifetimeMax;
	m_LifeSpanMin = a_LifetimeMin;

	m_Particles = new ParticleGPU[a_MaxParticles];

	m_ActiveBuffer = 0;

	CreateBuffers();

	CreateUpdateShader();

	CreateDrawShader();
}

void GPUParticle::CreateBuffers()
{
	//create buffers
	glGenVertexArrays(2, m_VAO);
	glGenBuffers(2, m_VBO);

	//bind buffers
	glBindVertexArray(m_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, m_MaxParticles * sizeof(ParticleGPU), m_Particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), ((char *)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), ((char *)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), ((char *)0) + 28);

	glBindVertexArray(m_VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, m_MaxParticles * sizeof(ParticleGPU), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), ((char *)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), ((char *)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPU), ((char *)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUParticle::CreateDrawShader()
{
	unsigned int VertShader = Utility::loadShader("./Resources/Shaders/Particles.vert", GL_VERTEX_SHADER);
	unsigned int FragShader = Utility::loadShader("./Resources/Shaders/Particles.frag", GL_FRAGMENT_SHADER);
	unsigned int GeomShader = Utility::loadShader("./Resources/Shaders/Particles.geom", GL_GEOMETRY_SHADER);

	const char* cInputs[] = { "Position", "Velocity", "Lifetime", "Lifespan" };
	m_DrawShader = Utility::createProgram(VertShader, 0, 0, GeomShader, FragShader, 4, cInputs);

	glDeleteShader(VertShader);
	glDeleteShader(FragShader);
	glDeleteShader(GeomShader);

	glUseProgram(m_DrawShader);

	//bind the size information
	unsigned int Location = glGetUniformLocation(m_DrawShader, "sizeStart");
	glUniform1f(Location, m_StartSize);

	Location = glGetUniformLocation(m_DrawShader, "sizeEnd");
	glUniform1f(Location, m_EndSize);

	//bind the color information
	Location = glGetUniformLocation(m_DrawShader, "colorStart");
	glUniform4fv(Location, 1, glm::value_ptr(m_StartColor));

	Location = glGetUniformLocation(m_DrawShader, "colorEnd");
	glUniform4fv(Location, 1, glm::value_ptr(m_EndColor));
}

void GPUParticle::CreateUpdateShader()
{
	unsigned char* Source = Utility::fileToBuffer("./Resources/Shaders/UpdateParticles.vert");

	//create a shader
	unsigned int VertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertShader, 1, (const char**)&Source, 0);
	glCompileShader(VertShader);

	delete[] Source;

	m_UpdateShader = glCreateProgram();
	glAttachShader(m_UpdateShader, VertShader);

	glBindAttribLocation(m_UpdateShader, 0, "Position");
	glBindAttribLocation(m_UpdateShader, 1, "Velocity");
	glBindAttribLocation(m_UpdateShader, 2, "Lifetime");
	glBindAttribLocation(m_UpdateShader, 3, "Lifespan");

	const char* Varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_UpdateShader, 4, Varyings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(m_UpdateShader);

	glDeleteShader(VertShader);

	glUseProgram(m_UpdateShader);

	unsigned int Location = glGetUniformLocation(m_UpdateShader, "lifeMin");
	glUniform1f(Location, m_LifeSpanMin);

	Location = glGetUniformLocation(m_UpdateShader, "lifeMax");
	glUniform1f(Location, m_LifeSpanMax);
}

void GPUParticle::Draw(const glm::mat4& a_CameraTransform, const glm::mat4& a_Projection)
{
	glUseProgram(m_UpdateShader);

	unsigned int Location = glGetUniformLocation(m_UpdateShader, "time");
	glUniform1f(Location, Utility::getTotalTime());

	Location = glGetUniformLocation(m_UpdateShader, "deltaTime");
	glUniform1f(Location, Utility::getDeltaTime());

	Location = glGetUniformLocation(m_UpdateShader, "emitterPosition");
	glUniform3fv(Location, 1, glm::value_ptr(m_Position));
	
	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_VAO[m_ActiveBuffer]);

	unsigned int OtherBuffer = (m_ActiveBuffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBO[OtherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_MaxParticles);

	//disable the transform feedback and reenable rasterization
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glUseProgram(m_DrawShader);

	glm::mat4 ViewMatrix = glm::inverse(a_CameraTransform);

	Location = glGetUniformLocation(m_DrawShader, "ProjectionView");
	glUniformMatrix4fv(Location, 1, false, glm::value_ptr(a_Projection * ViewMatrix));

	Location = glGetUniformLocation(m_DrawShader, "CameraTransform");
	glUniformMatrix4fv(Location, 1, false, glm::value_ptr(a_CameraTransform));

	//draw particles using the other buffer
	glBindVertexArray(m_VAO[OtherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_MaxParticles);

	//swap for the next frame
	m_ActiveBuffer = OtherBuffer;

}