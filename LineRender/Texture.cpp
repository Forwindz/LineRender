#include "Texture.h"

Render::Texture::~Texture()
{
	//if (texture != -1)glDeleteTextures(1, &texture);
}

bool Render::Texture::LoadTexture(GLenum target, const char * fileName)
{
	Magick::Image m_image;
	Magick::Blob m_blob;
	m_textureTarget = target;
	try {
		m_image.read(fileName);
		m_image.write(&m_blob, "RGBA");
	}
	catch (Magick::Error& Error) 
	{
		fprintf(stderr, "cannot read picture %s : %s", fileName,Error.what());
		return false;
	}
	glGenTextures(1, &texture);
	glBindTexture(target, texture);
	auto a = m_blob.data();
	glTexImage2D(target, 0, GL_RGBA, m_image.columns(), m_image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
	glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(target, 0);
	return true;
}

void Render::Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget,texture);
}
