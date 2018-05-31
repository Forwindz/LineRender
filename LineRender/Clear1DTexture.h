#pragma once
#include "PreCompile.h"
#include "ListTexture.h"
namespace Render
{
	class Clear1DTexture
	{
	public:
		Clear1DTexture();
		~Clear1DTexture();
		void InitDefaultData(void * dataPtr, int length, int dataLen, GLenum formatType, GLenum pixelType);
		void setDefaultData(GLuint texture);
		void clear();
	private:
		GLuint defaultBuffer=-1;
		GLenum pixelType, formatType;
		int imageLen;
	};
}