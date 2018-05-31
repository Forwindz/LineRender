#include "PreCompile.h"
#include "LineStripTech.h"
#include "Control.h"
#include "TimeTool.h"
#include "Light.h"
Shape::LineStrip lineStrip;
Render::Camera camera(glm::vec3(100, 0, 0), 0, 130.0f,
	(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
	5000.0f, 1.0f, 150.0f);
Input::MouseControl mouse;
Tool::FPSCalculator fps;
Render::LightComponent light;

Render::LineTech lineTech;



static void getParams()
{
	while (1)
	{
		std::cout<<"Params pqrls:\n";
		float p, q, r, s, l;
		std::cin >> p >> q >> r >> l >> s;
		if (p <= -0.01f)
		{
			printf("exit");
			exit(0);
		}
		lineTech.setParams(p, q, r, l, s);
		std::cout << ("-------\n");
	}
}

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

static void PrepareData()
{
	std::cout << "Input file path= ";
	std::string filePath;
	std::cin >> filePath;
	//std::string filePath = "F:\\projects\\vs2017\\LineRender\\data\\tornado.txt";
	lineStrip.LoadFromFile(filePath,0.05f);
	std::cout << "data generated.\n";
	lineTech.Prepare(lineStrip, { 1.0f ,2000.0f/2.0f ,2000.0f/2.0f ,2.0f },20.0f);
	std::cout << "prepared.\n";
	CheckOpenGLError("prepare Data end");
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

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_PRIMITIVE_RESTART);

#ifndef _OPENMP
	fprintf(stderr, "OpenMP not supported");
#endif

	lineTech.Init();
	lineTech.Enable();
	camera.Init(lineTech.GetProgram());
	CheckOpenGLError("LineTech init end");
	PrepareData();
	//generate data
	/*
	Shape::RandomGenerateLineStrip(lineStrip, glm::vec3(1.0f, 0.5f, 0.1f), 0.12f, 400, 20, 3000);
	CheckOpenGLError("Line generate");
	lineTech.Prepare(lineStrip, {2.0f/2,0.6f/2,0.06f/2,1.0f/2,2.5f/2});
	CheckOpenGLError("Line pre end");*/
	mouse.Init(camera);
	fps.Init();
	
	light.data.al.color = WHITE;
	light.data.al.intensity = 0.4f;
	light.data.dl.color = WHITE;
	light.data.dl.direct = glm::normalize(glm::vec3(1.0f,0.5f,0.2f));
	light.data.dl.intensity = 0.6f;
	
	light.Init(lineTech.GetProgram());

	std::thread th(getParams);
	th.detach();
	glutMainLoop();
	return 0;
}