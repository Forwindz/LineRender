#include "ListTexture.h"

Render::ListTexture::ListTexture()
{
}

Render::ListTexture::~ListTexture()
{
	clear();
}

void Render::ListTexture::InitData(GLenum textureType, GLsizeiptr len, GLenum bufferType)
{
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_TEXTURE_BUFFER, buffer);
	glBufferData(GL_TEXTURE_BUFFER, len, NULL, bufferType);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_BUFFER, texture);
	glTexBuffer(GL_TEXTURE_BUFFER, textureType, buffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	this->len = len;
	this->textureType = textureType;
}

void Render::ListTexture::SetData(const void * dataPtr)
{
	glBindBuffer(GL_TEXTURE_BUFFER, buffer);
	char* alphaPointer = (char*)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
	memcpy(alphaPointer, dataPtr, len);
	glUnmapBuffer(GL_TEXTURE_BUFFER);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void Render::ListTexture::GetData(void * dataPtr)
{
	glBindBuffer(GL_TEXTURE_BUFFER, buffer);
	char* alphaPointer = (char*)glMapBuffer(GL_TEXTURE_BUFFER, GL_READ_ONLY);
	memcpy(dataPtr, alphaPointer, len);
	glUnmapBuffer(GL_TEXTURE_BUFFER);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void Render::ListTexture::Bind(GLuint unit, GLenum mode)
{
	glBindImageTexture(unit, texture, 0, GL_FALSE, 0, mode, textureType);
}

void Render::ListTexture::clear()
{
	if (buffer != -1)
		glDeleteBuffers(1, &buffer);
	if (texture != -1)
		glDeleteTextures(1, &texture);
	buffer = texture = -1;
}
