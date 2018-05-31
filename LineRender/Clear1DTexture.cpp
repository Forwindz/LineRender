#include "Clear1DTexture.h"

Render::Clear1DTexture::Clear1DTexture()
{
}

Render::Clear1DTexture::~Clear1DTexture()
{
	clear();
}

void Render::Clear1DTexture::InitDefaultData(void * dataPtr, int length, int dataLen, GLenum formatType, GLenum pixelType)
{
	clear();
	imageLen = length;
	this->pixelType = pixelType;
	this->formatType = formatType;
	CheckOpenGLError("a1000");
	glGenBuffers(1, &defaultBuffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, defaultBuffer);
	CheckOpenGLError("a3");
	glBufferData(GL_PIXEL_UNPACK_BUFFER, length*dataLen, NULL, GL_STATIC_DRAW);
	CheckOpenGLError("a4");
	void* data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	memcpy(data, dataPtr, length*dataLen);
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	CheckOpenGLError("a5");
}

void Render::Clear1DTexture::setDefaultData(GLuint texture)
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, defaultBuffer);
	CheckOpenGLError("a");
	glBindTexture(GL_TEXTURE_1D, texture);
	CheckOpenGLError("a1");
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, imageLen, formatType, pixelType, NULL);
	CheckOpenGLError("a2");
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Render::Clear1DTexture::clear()
{
	if (defaultBuffer != -1)glDeleteBuffers(1, &defaultBuffer);
	defaultBuffer = -1;
}
