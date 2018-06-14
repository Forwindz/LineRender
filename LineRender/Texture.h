#pragma once
#include "PreCompile.h"

namespace Render 
{
	class Texture
	{
	public:
		~Texture();
		bool LoadTexture(GLenum TextureTarget, const char* fileName);
		void Bind(GLenum TextureUnit);
		GLuint texture=-1;
	private:
		GLenum m_textureTarget;
	};
}

