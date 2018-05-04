#include "TransMatrix.h"

void Render::Translation(glm::mat4x4 & mat, const float x, const float y, const float z)
{
	mat[0][3] = x;
	mat[1][3] = y;
	mat[2][3] = z;
}

void Render::Rotation(glm::mat4x4 & mat,const int rotateDirection, const float rad)
{
	switch (rotateDirection)
	{
	case 0://x
		mat[1][1] = cosf(rad);
		mat[2][1] = sinf(rad);
		mat[1][2] = -sinf(rad);
		mat[2][2] = cosf(rad);
		break;
	case 1://y
		mat[0][0] = cosf(rad);
		mat[2][0] = sinf(rad);
		mat[0][2] = -sinf(rad);
		mat[2][2] = cosf(rad);
		break;
	case 2://z
		mat[0][0] = cosf(rad);
		mat[1][0] = sinf(rad);
		mat[0][1] = -sinf(rad);
		mat[1][1] = cosf(rad);
		break;
	}
}

void Render::Magnification(glm::mat4x4 & mat, const float x, const float y, const float z)
{
	mat[0][0] = x;
	mat[1][1] = y;
	mat[2][2] = z;
}

void Render::NoTranslation(glm::mat4x4 & mat)
{
	memset(&mat, 0, sizeof(mat));
	mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1;
}

void Render::PerspectiveProj(glm::mat4x4 & mat, const float ar, const float zNear, const float zFar, const float fov)
{
	const float zRange = zNear - zFar;
	const float cotHalfFOV = 1.0f/(float)tanf(fov / 2.0);

	mat[0][0] = cotHalfFOV / ar;
	mat[0][1] = 0.0f;
	mat[0][2] = 0.0f;
	mat[0][3] = 0.0f;

	mat[1][0] = 0.0f;
	mat[1][1] = cotHalfFOV;
	mat[1][2] = 0.0f;
	mat[1][3] = 0.0f;

	mat[2][0] = 0.0f;
	mat[2][1] = 0.0f;
	mat[2][2] = (-zNear - zFar) / zRange;
	mat[2][3] = 2.0f * zFar * zNear / zRange;

	mat[3][0] = 0.0f;
	mat[3][1] = 0.0f;
	mat[3][2] = 1.0f;
	mat[3][3] = 0.0f;
}

void Render::PerspectiveProj(glm::mat4x4& mat, PerspectiveAttribute & attr)
{
	PerspectiveProj(mat, attr.ar, attr.zNear, attr.zFar, attr.fov);
}
/*
void Render::ViewMatrix(glm::mat4x4 & mat, float verticalDeg, float horizonDeg)
{
	const glm::vec3 Vaxis(0.0f, 1.0f, 0.0f);
	glm::vec3 View(0.0f, 0.0f, 1.0f);
	RotateVector(View, horizontalDeg, Vaxis);
	viewVec = View = glm::normalize(View);

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
}*/

void Render::RotateVector(glm::vec3& vec, float deg, glm::vec3 axis)
{
	const float sind = sin(deg / 180.0f / 2.0f*3.1415926535f);
	const float cosd = cos(deg / 180.0f / 2.0f*3.1415926535f);
	glm::quat t(cosd, sind*axis.x, sind*axis.y, sind*axis.z);
	vec = t*vec*glm::conjugate(t);

}
