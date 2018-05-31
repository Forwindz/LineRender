#include "Params.h"

const static int NumName = 4;
static const char* names[NumName] =
{
	"params.p",
	"params.q",
	"params.r",
	"params.l"
};

static const int fcounts[NumName] =
{
	1,1,1,1
};


void pushUData2(const int n, GLuint loc, float* data)
{
	switch (n)
	{
	case 1:
		glUniform1f(loc, data[0]);
		break;
	case 2:
		glUniform2f(loc, data[0], data[1]);
		break;
	case 3:
		glUniform3f(loc, data[0], data[1], data[2]);
		break;
	}
}
void Render::ParamUniform::Init(GLuint program)
{

	for (int i = 0; i < NumName; i++)
	{
		pos[i] = glGetUniformLocation(program, names[i]);
		CheckOpenGLError("Params Pre " + (char)(i + 97));
	}

}

void Render::ParamUniform::UpdateData()
{
	float* tempPtr = (float*)&data;
	for (int i = 0; i < NumName; i++)
	{
		pushUData2(fcounts[i], pos[i], tempPtr);
		tempPtr += fcounts[i];
	}

}