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

	struct SpecularLight
	{
		glm::vec3 eyePos;
		float intensity;
		float power;
	};

	struct LightData
	{
		AmbientLight al;
		DirectionLight dl;
		SpecularLight sl;
	};

	class LightComponent
	{
	public:
		void Init(GLuint program);
		void UpdateData();
		LightData data;
	private:
		const static int NumName = 8;
		GLuint pos[NumName];
	};
}