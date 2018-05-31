#pragma once
#include "PreCompile.h"
namespace Render
{
	struct Param
	{
		float p, q, r, l;
	};

	class ParamUniform
	{
	public:
		void Init(GLuint program);
		void UpdateData();
		Param data;
	private:
		const static int NumName = 4;
		GLuint pos[NumName];
	};
}