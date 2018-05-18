#include "PreCompile.h"
#include "LineTech.h"
#include "Control.h"
#include "TimeTool.h"
#include "Light.h"
Shape::LineStrip lineStrip;
Render::Camera camera(glm::vec3(600, 0, 100), 0, 130.0f,
	(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
	5000.0f, 1.0f, 150.0f);
Input::MouseControl mouse;
Tool::FPSCalculator fps;
Render::LightComponent light;

Render::LineTech lineTech;

static void RenderScenceCB()
{
	
	glEnable(GL_MULTISAMPLE);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	//camera.horizontalDeg += 0.001f;
	camera.Recomp();
	//light.UpdateData();
	lineTech.Render(lineStrip, camera,light);

	glFlush();
	glutSwapBuffers();
	CheckOpenGLError("Render");

	fps.Tick();
	std::string str = "Line Render   FPS=" + std::to_string((int)fps.GetFPS());
		/*+" \t\t Average FPS="+std::to_string((int)fps.GetAverageFPS()*/
	glutSetWindowTitle(str.c_str());
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderScenceCB);
	// ����Ⱦ�ص�ע��Ϊȫ�����ûص�
	glutIdleFunc(RenderScenceCB);
}

int main(int argc, char ** argv)
{
	// ��ʼ��GLUT
	glutInit(&argc, argv);

	// ��ʾģʽ��˫���塢RGBA
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// ��������
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);      // ���ڳߴ�
	glutInitWindowPosition(100, 100);  // ����λ��
	glutCreateWindow("Line Render");   // ���ڱ���

	InitializeGlutCallbacks();

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_PRIMITIVE_RESTART);

#ifndef _OPENMP
	fprintf(stderr, "OpenMP not supported");
#endif

	lineTech.Init();
	lineTech.Enable();
	//camera.horizontalDeg = 1.0f;
	camera.Init(lineTech.GetProgram());
	CheckOpenGLError("LineTech init end");
	//generate data
	Shape::RandomGenerateLineStrip(lineStrip, glm::vec3(1.0f, 0.5f, 0.1f), 0.1f, 500, 60, 3000);
	CheckOpenGLError("Line generate");
	lineTech.Prepare(lineStrip, {1.0f/2,0.6f/2,0.06f/2,1.0f/2,2.5f/2});
	CheckOpenGLError("Line pre end");
	mouse.Init(camera);
	fps.Init();
	
	light.data.al.color = WHITE;
	light.data.al.intensity = 0.4f;
	light.data.dl.color = WHITE;
	light.data.dl.direct = glm::normalize(glm::vec3(1.0f,0.5f,0.2f));
	light.data.dl.intensity = 0.6f;
	//memset(&(light.data), 255, sizeof(light.data));
	light.Init(lineTech.GetProgram());
	glutMainLoop();
	return 0;
}