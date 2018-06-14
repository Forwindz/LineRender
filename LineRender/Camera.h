#pragma once
#include "PreCompile.h"
#include "TransMatrix.h"
namespace Render
{

	
	class Camera
	{
	public:
		Camera(glm::vec3 _pos,float _verticalDeg, float _horizontalDeg,
			float ar, float zFar, float zNear, float fov);
		Camera();
		//相机位置
		glm::vec3 pos;
		//角度
		float verticalDeg, horizontalDeg;
		//视角方向
		glm::vec3 viewVec;
		//透视
		PerspectiveAttribute perspect;
		//获得矩阵，包括：相机位移、相机旋转、透视
		glm::mat4x4 wvpMat, worldMat, viewMat, projectionMat;
		//距离上次更新延时多久以后才算更新完毕（避免用户一直拖啊拖）
		int updateAvaliableTime;
		//中心点，可以设置围绕中心点旋转
		glm::vec3 centerPos;
		//更新uniform的data
		void UpdateData();
		void Init(GLuint ShaderProgram);
		void Recomp();
		//移动摄像机，移动方向以当前视觉方向为参考
		void Move(const glm::vec3 t);
		bool isUpdating() const;
		void notifyUpdated();
	private:
		GLuint wvpMatLoc;
		GLuint worldMatLoc;
		int hasUpdate;
	};

	class CenterCameraControl
	{
	public:
		CenterCameraControl(Camera& camera);
		glm::vec3 centerPos;
		void RotateAround(const float verticalDeg, const float horizonDeg);
		void MoveDistance(const float moveDistance);
	private:
		Camera& camera;
	};
	

}