#pragma once
#include "PreCompile.h"

namespace Render
{

	struct AmbientLight
	{
		glm::vec3 color;
		float intensity;
	};

	struct DirectionLight
	{
		glm::vec3 color;
		float intensity;
		glm::vec3 direct;
	};

	struct LightData
	{
		AmbientLight al;
		DirectionLight dl;
	};

	class LightComponent
	{
	public:
		void Init(GLuint program);
		void UpdateData();
		LightData data;
	private:
		const static int NumName = 5;
		GLuint pos[NumName];
	};
}