#pragma once
#include "PreCompile.h"
#include "FileTools.h"
namespace Shader
{
	//从文件里加shader到program中
	GLuint AddShaderFromFile(GLuint program,const char* fileName, GLenum shaderType);
	//进行程序连接等操作
	void PrepareProgram(GLuint program);

	//建立着色器程序，包括编译，连接，检查
	GLuint CreateProgramSimple(const char* vs, const char* fs);

	//建立与Uniform block之间的联系
	GLuint GetUniformBlockBuffer(GLuint program, const char* name,int dataSize, GLenum dataType = GL_STATIC_DRAW);
}