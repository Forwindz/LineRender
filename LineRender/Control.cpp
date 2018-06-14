#include "Control.h"

Input::MouseControl* mouse_ptr;
bool mouse_isPressing;

void OnMouseClick(int button, int state, int x, int y)
{
	
	if (mouse_ptr == nullptr)return;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		mouse_ptr->mmm = Input::MouseMoveMethod::Rotate;
		break;
	case GLUT_RIGHT_BUTTON:
		mouse_ptr->mmm = Input::MouseMoveMethod::Move;
		break;
	default:
		break;
	}

	switch (state)
	{
	case GLUT_DOWN:
		mouse_isPressing = true;
		break;
	case GLUT_UP:
		mouse_isPressing = false;
		mouse_ptr->MouseUp();
		break;
	}
	//printf(">clicked\n");
}

void OnMouseMove(int x, int y)
{
	if (mouse_ptr == nullptr)return;
	//printf(">%d\n", mouse_isPressing);
	if(mouse_isPressing)mouse_ptr->UpdateMousePos(x, y);
}

Input::MouseControl::MouseControl()
{
	camera = nullptr;
	lastX = lastY = 0;
	xChangePosSpeed = yChangePosSpeed = 0.06f;
	xChangeRotateSpeed = yChangeRotateSpeed = 0.014f;
	zChangePosSpeed = 0.08f;
}

Input::MouseControl::~MouseControl()
{
	camera = nullptr;
	mouse_ptr = nullptr;
}

void Input::MouseControl::Init(Render::Camera & _camera)
{
	camera = &_camera;
	glutMotionFunc(OnMouseMove);
	glutMouseFunc(OnMouseClick);
	mouse_ptr = this;
	first = true;
	hasOperate = false;
}

void Input::MouseControl::MoveCamera(const int dx, const int dy)
{
	//printf(">move(%d,%d)\n",dx,dy);
	const int z = (abs(dx) > 1 && abs(dy) > 1) ? (dx + dy) / 2 : 0;
	if (z == 0)
		camera->Move(glm::vec3(dx*xChangePosSpeed, dy*yChangePosSpeed,0));
	else
		camera->Move(glm::vec3(0,0, z*zChangePosSpeed));
	hasOperate = true;
}

void Input::MouseControl::RotateCamera(const int dx, const int dy)
{
	//printf(">rotate(%d,%d)\n", dx, dy);
	camera->horizontalDeg += dx * xChangeRotateSpeed;
	camera->verticalDeg += dy * yChangeRotateSpeed;
	hasOperate = true;
	camera->notifyUpdated();
}

void Input::MouseControl::UpdateMousePos(int newX, int newY)
{
	int dx = newX - lastX;
	int dy = newY - lastY;
	lastX = newX;
	lastY = newY;
	if (first)
	{
		first = false;
		return;
	}
	if (mmm==MouseMoveMethod::Rotate)
		RotateCamera(dx, dy);
	else if(mmm== MouseMoveMethod::Move)
		MoveCamera(dx, dy);
}

void Input::MouseControl::MouseUp()
{
	first = true;
	hasOperate = true;
}

bool Input::MouseControl::hasOperated()
{
	bool temp = hasOperate;
	hasOperate = false;
	return temp;
}
//======================================
//keyboard
//======================================

Input::KeyboardControl* kbc = nullptr;

void processNormalKeys(unsigned char key, int x, int y)
{
	if (kbc == nullptr)return;
	switch (key)
	{
	case 'w':
		kbc->InputTrans(1, 0);
		break;
	case 's':
		kbc->InputTrans(-1, 0);
		break;
	case 'a':
		kbc->InputTrans(0, 1);
		break;
	case 'd':
		kbc->InputTrans(0, -1);
		break;
	}
}

Input::KeyboardControl::KeyboardControl()
{

}

Input::KeyboardControl::~KeyboardControl()
{
	kbc = nullptr;
}

void Input::KeyboardControl::Init(Render::CenterCameraControl* ccc)
{
	this->ccc = ccc;
	horSpeed = 1.0f;
	verSpeed = 1.0f;
	kbc = this;
	glutKeyboardFunc(processNormalKeys);
}

void Input::KeyboardControl::InputTrans(const float ver, const float hor)
{
	ccc->RotateAround(ver*verSpeed, hor*horSpeed);
}
