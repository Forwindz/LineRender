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
	// 将渲染回调注册为全局闲置回调
	glutIdleFunc(RenderScenceCB);
}

int main(int argc, char ** argv)
{
	// 初始化GLUT
	glutInit(&argc, argv);

	// 显示模式：双缓冲、RGBA
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// 窗口设置
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);      // 窗口尺寸
	glutInitWindowPosition(100, 100);  // 窗口位置
	glutCreateWindow("Line Render");   // 窗口标题

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
	lineTech.Prepare(lineStrip, {0.5f,0.3f,0.03f,0.5f,0.25f});
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