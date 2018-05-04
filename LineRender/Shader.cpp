#include "Shader.h"

GLuint Shader::AddShaderFromFile(GLuint ShaderProgram, const char * fileName, GLenum ShaderType)
{
	// 根据shader类型参数定义两个shader对象
	GLuint ShaderObj = glCreateShader(ShaderType);
	// 检查是否定义成功
	if (ShaderObj == 0) 
	{
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		PAUSE_SHOW_ERROR
	}

	// 定义shader的代码源
	GLint lengths[1];
	const GLchar* p[1];
	p[0] = FileTool::readWholeFile(fileName, lengths[0]);
#ifdef DEBUG
	printf("=====shader program====== %s || %d chars\n", fileName,lengths[0]);
	printf("%s\n", p[0]);
#endif // DEBUG
	glShaderSource(ShaderObj, 1, p, lengths);
	glCompileShader(ShaderObj);// 编译shader对象

							   // 检查和shader相关的错误
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) 
	{
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		PAUSE_SHOW_ERROR
	}

	// 将编译好的shader对象绑定到program object程序对象上
	glAttachShader(ShaderProgram, ShaderObj);

	delete[] p[0];
	return ShaderObj;
}

void Shader::PrepareProgram(GLuint ShaderProgram)
{
	// 链接shader着色器程序，并检查程序相关错误
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) 
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		PAUSE_SHOW_ERROR
	}

	// 检查验证在当前的管线状态程序是否可以被执行
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) 
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		PAUSE_SHOW_ERROR
	}
}

GLuint Shader::CreateProgramSimple(const char * vs, const char * fs)
{
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) 
	{
		fprintf(stderr, "Error creating shader program\n");
		PAUSE_SHOW_ERROR
		return 0;
	}
	AddShaderFromFile(ShaderProgram, vs, GL_VERTEX_SHADER);
	AddShaderFromFile(ShaderProgram, fs, GL_FRAGMENT_SHADER);
	PrepareProgram(ShaderProgram);
	return ShaderProgram;
}

GLuint Shader::GetUniformBlockBuffer(
	GLuint program, const char * name, 
	int dataSize, GLenum dataType)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, dataType);
	return buffer;
}
