#include "Light.h"

const static int NumName = 8;
static const char* names[NumName] =
{
	"lights.al.color",
	"lights.al.intensity",
	"lights.dl.color",
	"lights.dl.intensity",
	"lights.dl.direct",
	"lights.sl.eyePos",
	"lights.sl.intensity",
	"lights.sl.power"
};

static const int fcounts[NumName] =
{
	3,1,3,1,3,3,1,1
};


void pushUData(const int n, GLuint loc, float* data)
{
	switch (n)
	{
	case 1:
		glUniform1f(loc, data[0]);
		break;
	case 2:
		glUniform2f(loc, data[0],data[1]);
		break;
	case 3:
		glUniform3f(loc, data[0], data[1], data[2]);
		break;
	}
}
void Render::LightComponent::Init(GLuint program)
{
	
	for (int i = 0; i < NumName; i++)
	{
		pos[i]=glGetUniformLocation(program, names[i]);
		CheckOpenGLError("Light Tech Pre "+(char)(i+97));
	}
	
}

void Render::LightComponent::UpdateData()
{
	float* tempPtr = (float*)&data;
	for (int i = 0; i < NumName; i++)
	{
		pushUData(fcounts[i], pos[i], tempPtr);
		tempPtr += fcounts[i];
	}

}
