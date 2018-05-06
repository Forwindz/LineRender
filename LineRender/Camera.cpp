#include "Camera.h"

Render::Camera::Camera(glm::vec3 _pos, float _verticalDeg, float _horizontalDeg, 
	float ar, float zFar, float zNear, float fov):
	pos(_pos),verticalDeg(_verticalDeg),horizontalDeg(_horizontalDeg)
{
	perspect.ar = ar;
	perspect.fov = fov;
	perspect.zFar = zFar;
	perspect.zNear = zNear;
}

Render::Camera::Camera()
{
	pos = { 0.0f,0.0f,0.0f };
	verticalDeg = horizontalDeg = 0;
	perspect.ar = 1.0f;
	perspect.fov = 150;
	perspect.zFar = 0.1f;
	perspect.zNear = 10.0f;
}

void Render::Camera::UpdateData()
{
	glUniformMatrix4fv(wvpMatLoc, 1, GL_TRUE, (const GLfloat*)(&wvpMat));
	glUniformMatrix4fv(worldMatLoc, 1, GL_TRUE, (const GLfloat*)(&worldMat));
}

void Render::Camera::Init(GLuint ShaderProgram)
{
	wvpMatLoc = glGetUniformLocation(ShaderProgram, "gWVP");
	worldMatLoc = glGetUniformLocation(ShaderProgram, "gWorld");
	hasUpdate = 0;
	updateAvaliableTime = 20;
}

void Render::Camera::Recomp()
{
	if(hasUpdate<INT_MAX)hasUpdate++;
	NoTranslation(viewMat);
	NoTranslation(projectionMat);
	NoTranslation(worldMat);
	
	//World
	Translation(worldMat, -pos.x, -pos.y, -pos.z);
	//View
	glm::vec3 u,v,n;

	const glm::vec3 Vaxis(0.0f, 1.0f, 0.0f);
	glm::vec3 View(0.0f, 0.0f, 1.0f);
	RotateVector(View, horizontalDeg, Vaxis);
	viewVec=View=glm::normalize(View);

	glm::vec3 Haxis = glm::cross(Vaxis, View);
	RotateVector(View, verticalDeg, Haxis);

	n = View;
	u = glm::cross(Haxis, n);
	v = glm::cross(u, n);
	v = glm::normalize(v);
	viewMat =
	{
		v.x,v.y,v.z,0.0f,
		u.x,u.y,u.z,0.0f,
		n.x,n.y,n.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	//м╦йс
	Render::PerspectiveProj(projectionMat, perspect);
	wvpMat = worldMat*viewMat*projectionMat;
}

void Render::Camera::Move(const glm::vec3 t)
{
	const glm::vec4 trans(t.x, t.y, t.z, 1.0f);
	auto temp = trans * viewMat;
	pos += glm::vec3(temp.x, temp.y, temp.z);
	hasUpdate = true;
}

bool Render::Camera::HasUpdated() const
{
	return hasUpdate<updateAvaliableTime;
}

void Render::Camera::notifyUpdated()
{
	hasUpdate = 0;
}
