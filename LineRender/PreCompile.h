#pragma once
#pragma warning(disable:4251)
#pragma warning(disable:4275)

#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <time.h>

#include <glew\glew.h>
#include <freeglut\freeglut.h>

#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\mat3x3.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\quaternion.hpp>

#include <vector>
#include <string>
#include <array>
#include <map>

#include <iostream>

#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <assimp\Importer.hpp>

#include <Magick++.h>

#include <armadillo.h>

#ifdef WIN32
	#define HAVE_STRUCT_TIMESPEC
	#define msleep(x) Sleep(x)
#endif

#include <pthread.h>

static const glm::vec3 WHITE(1.0f, 1.0f, 1.0f);
static const glm::vec3 BLACK(0.0f, 0.0f, 0.0f);

#define DEBUG

#ifdef DEBUG
	#define PAUSE_SHOW_ERROR {system("pause");exit(1);}
#else
	#define PAUSE_SHOW_ERROR {exit(1);}
#endif

#define RandFloat() (rand()/double(RAND_MAX)-0.5) 


static const int SCREEN_HEIGHT = 1000;
static const int SCREEN_WIDTH = 1400;

void _checkOpenGLErrorInternal(const char* ch);

#ifdef DEBUG
	#define CheckOpenGLError(info) _checkOpenGLErrorInternal(info)
#else
	#define CheckOpenGLError(info) 
#endif

