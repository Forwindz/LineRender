#pragma once
#include "PreCompile.h"
#include "Shader.h"
namespace Render
{
	class Tech
	{
	public:
		Tech();
		virtual ~Tech();
		virtual bool Init();
		virtual bool Update();
		void Clear();
		void Enable();
		GLuint& GetProgram();
	protected:
		typedef std::vector<GLuint> ShaderList;
		ShaderList shaders;
		GLuint program;
	};
}

