#include "Tech.h"

Render::Tech::Tech()
{
}

Render::Tech::~Tech()
{
	Clear();
}

bool Render::Tech::Init()
{
	program = 0;
	program = glCreateProgram();

	if (program == 0) {
		fprintf(stderr, "Error creating shader program\n");
		PAUSE_SHOW_ERROR
		return false;
	}

	return true;
}

bool Render::Tech::Update()
{
	return false;
}

void Render::Tech::Clear()
{
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		glDeleteShader(*i);
	}
	if (program != 0)
	{
		glDeleteProgram(program);
		program = 0;
	}
}

void Render::Tech::Enable()
{
	glUseProgram(program);
}

GLuint & Render::Tech::GetProgram()
{
	return program;
}
