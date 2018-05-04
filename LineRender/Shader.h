#pragma once
#include "PreCompile.h"
#include "FileTools.h"
namespace Shader
{
	//���ļ����shader��program��
	GLuint AddShaderFromFile(GLuint program,const char* fileName, GLenum shaderType);
	//���г������ӵȲ���
	void PrepareProgram(GLuint program);

	//������ɫ�����򣬰������룬���ӣ����
	GLuint CreateProgramSimple(const char* vs, const char* fs);

	//������Uniform block֮�����ϵ
	GLuint GetUniformBlockBuffer(GLuint program, const char* name,int dataSize, GLenum dataType = GL_STATIC_DRAW);
}