#pragma once
#include "PreCompile.h"

namespace Render
{
	class ListTexture
	{
	public:
		ListTexture();
		~ListTexture();
		void InitData(
			GLenum textureType,
			GLsizeiptr len,
			GLenum bufferType = GL_DYNAMIC_COPY);
		void SetData(const void* dataPtr);
		void GetData(void* dataPtr);
		void Bind(GLuint unit, GLenum mode = GL_READ_WRITE);
		GLuint buffer=-1, texture=-1;
	private:
		GLsizeiptr len;
		GLenum textureType;
		void clear();
	};
}