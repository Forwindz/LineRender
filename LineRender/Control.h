#pragma once
#include "PreCompile.h"
#include "Camera.h"

namespace Input
{
	enum MouseMoveMethod
	{
		Disable,
		Rotate,
		Move
	};

	class MouseControl
	{
	public:
		MouseControl();
		~MouseControl();
		void Init(Render::Camera& _camera);
		void MoveCamera(const int dx,const int dy);
		void RotateCamera(const int dx, const int dy);
		void UpdateMousePos(int newX, int newY);
		void MouseUp();
		//�ж�����Ƿ��в���������true������
		bool hasOperated();
		float xChangePosSpeed, yChangePosSpeed, zChangePosSpeed;
		float xChangeRotateSpeed, yChangeRotateSpeed;
		MouseMoveMethod mmm;
	protected:
		Render::Camera* camera;
		bool first;
		bool hasOperate;
		int lastX, lastY;
	};
}