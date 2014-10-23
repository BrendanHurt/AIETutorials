#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>



// derived application class that wraps up all globals neatly
class Tutorial3 : public Application
{
public:

	Tutorial3();
	virtual ~Tutorial3();

protected:

	unsigned int m_Shader;

	FBXFile *m_pFBX;

	void InitFBXSceneResource(FBXFile *a_pScene);
	void UpdateFBXSceneResource(FBXFile *a_pScene);
	void RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_View, glm::mat4 a_Projeciton);
	void DestroyFBXSceneResource(FBXFile *a_pScene);

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	struct OGL_FBXRenderData
	{
		unsigned int VBO; //vertex buffer object
		unsigned int VAO; //vertex array object
		unsigned int IBO; //index buffer object
	};
};