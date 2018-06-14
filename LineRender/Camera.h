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
		//���λ��
		glm::vec3 pos;
		//�Ƕ�
		float verticalDeg, horizontalDeg;
		//�ӽǷ���
		glm::vec3 viewVec;
		//͸��
		PerspectiveAttribute perspect;
		//��þ��󣬰��������λ�ơ������ת��͸��
		glm::mat4x4 wvpMat, worldMat, viewMat, projectionMat;
		//�����ϴθ�����ʱ����Ժ���������ϣ������û�һֱ�ϰ��ϣ�
		int updateAvaliableTime;
		//���ĵ㣬��������Χ�����ĵ���ת
		glm::vec3 centerPos;
		//����uniform��data
		void UpdateData();
		void Init(GLuint ShaderProgram);
		void Recomp();
		//�ƶ���������ƶ������Ե�ǰ�Ӿ�����Ϊ�ο�
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