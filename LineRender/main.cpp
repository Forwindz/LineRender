#include "PreCompile.h"
#include "LineStripTech.h"
#include "Control.h"
#include "TimeTool.h"
#include "Light.h"
#include "Texture.h"
Shape::LineStrip lineStrip;
Render::Camera camera(glm::vec3(100, 0, 0), 0, 130.0f,
	(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
	5000.0f, 1.0f, 150.0f);
Render::CenterCameraControl ccc(camera);
Input::MouseControl mouse;
Input::KeyboardControl keyboard;
Tool::FPSCalculator fps;
Render::LightComponent light;

Render::LineTech lineTech;

static float regular(const float x)
{
	if (x < 0.0f)return 0.0f;
	if (x > 1.0f)return 1.0f;
	return x;
}

static void getParams()
{
	while (1)
	{
		std::string cmd;
		std::cin >> cmd;
		if (cmd == "p")
		{
			std::cout << "Params [p q r l s]\n";
			float p, q, r, s, l;
			std::cin >> p >> q >> r >> l >> s;
			lineTech.setParams(p, q, r, l, s);
		}
		else if (cmd == "dl")
		{
			std::cout << "direct light:[dir(xyz) intensity]\n";
			float x, y, z, its;
			std::cin >> x >> y >> z >> its;
			light.data.dl.direct = glm::normalize(glm::vec3(x, y, z));
			light.data.dl.intensity = regular(its);
		}
		else if (cmd == "al")
		{
			std::cout << "ambient light:[intensity]\n";
			float its;
			std::cin >> its;
			light.data.al.intensity = its;
		}
		else if (cmd == "sl")
		{
			std::cout << "specular light:[intensity,power]\n";
			float its, power;
			std::cin >> its >> power;
			light.data.sl.intensity = regular(its);
			light.data.sl.power = power;
		}
		else if (cmd == "q")
		{
			std::cout << "quit\n";
			exit(0);
		}
		
		std::cout << ("-------\n");
	}
}

static void RenderScenceCB()
{
	
	glEnable(GL_MULTISAMPLE);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	camera.Recomp();
	light.data.sl.eyePos = camera.pos;


	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);
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
	Render::Texture text;
	text.LoadTexture(GL_TEXTURE_2D, "texture\\line_texture.bmp");

	std::cout << "Input file path= ";
	std::string filePath;
	std::cin >> filePath;
	//std::string filePath = "F:\\projects\\vs2017\\LineRender\\data\\tornado.txt";
	if (filePath == "0")filePath = "data\\tornado.txt";
	else if (filePath == "1")filePath = "data\\rings.txt";
	else if (filePath == "2")filePath = "data\\heli.txt";
	std::cout << "Ready to load " << filePath << "\nInput total segments: ";
	int sn=-1;
	while (sn <= 0)std::cin >> sn;
	lineStrip.LoadFromFile(filePath, sn, 0.11f);
	std::cout << "data generated.\n";
	
	lineTech.Prepare(text, lineStrip, { 1.0f ,60 ,80 ,1 }, 10.0f);
	std::cout << "prepared.\n";
	CheckOpenGLError("prepare Data end");
}

static void InitAll()
{
	lineTech.Init();
	lineTech.Enable();
	camera.Init(lineTech.GetProgram());
	CheckOpenGLError("LineTech init end");
	PrepareData();
	ccc.centerPos = (lineStrip.max + lineStrip.min) / 2.0f;
	mouse.Init(camera);
	keyboard.Init(&ccc);
	fps.Init();

	light.data.al.color = WHITE;
	light.data.al.intensity = 0.2f;
	light.data.dl.color = WHITE;
	light.data.dl.direct = glm::normalize(glm::vec3(10.0f, 1.0f, 0.0f));
	light.data.dl.intensity = 0.9f;

	light.data.sl.intensity = 0.6f;
	light.data.sl.power = 20;

	light.Init(lineTech.GetProgram());

	std::thread th(getParams);
	th.detach();
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
	InitAll();
	
	glutMainLoop();
	return 0;
}