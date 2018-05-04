#pragma once
#include "PreCompile.h"
namespace Render
{
	void Translation(glm::mat4x4& mat, const float x, const float y, const float z);
	//1=x,2=y,3=z,others:ignore
	void Rotation(glm::mat4x4& mat, const int rotateDirection, const float rad);
	void Magnification(glm::mat4x4& mat, const float x, const float y, const float z);
	void NoTranslation(glm::mat4x4& mat);

	struct PerspectiveAttribute
	{
		float ar;
		float zNear;
		float zFar;
		float fov;
	};
	//ar=screen width/screen height, view distance: nearZ~farZ
	void PerspectiveProj(glm::mat4x4& mat,const float ar,const float nearZ,const float farZ, const float fvo);
	void PerspectiveProj(glm::mat4x4& mat, PerspectiveAttribute& attr);

	//void ViewMatrix(glm::mat4x4& mat, float verticalDeg, float horizonDeg);
	void RotateVector(glm::vec3& vec,float deg, glm::vec3 axis);
}