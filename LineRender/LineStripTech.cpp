#include "LineStripTech.h"
//meaningless
static const GLfloat uselessVertices[] =
{
	-1.0f, -1.0f,
	-1.0f, 1.0f,
	1.0f,  -1.0f,
	1.0f,  1.0f,
};
static const unsigned int uselessIndices[] =
{ 0,1,2,3 };

int Render::UniformStruct<Render::FadeParam>::numCount;
const std::string * Render::UniformStruct<Render::FadeParam>::names;
const int* Render::UniformStruct<Render::FadeParam>::fcounts;

Render::LineTech::LineTech()
{
	static const int fadeSize[] = { 1,1,1 };
	static const std::string fadeNames[] = { "fp.s","fp.it_times","fp.fade_speed" };
	UniformStruct<FadeParam>::numCount = 3;
	UniformStruct<FadeParam>::fcounts = fadeSize;
	UniformStruct<FadeParam>::names = fadeNames;
}

Render::LineTech::~LineTech()
{
	Release();

}


void Render::LineTech::Release()
{
	glDeleteTextures(1, &headTexture);

	glDeleteBuffers(1, &headClearBuffer);
	glDeleteBuffers(1, &atomCounterBuffer);
	glDeleteBuffers(1, &uselessVBO);
	glDeleteBuffers(1, &uselessIBO);

	glDeleteProgram(programSolve);
	glDeleteProgram(programCompute);

	if (initPtr != nullptr) delete[] initPtr;
}

bool Render::LineTech::Init()
{
	//shader for rendering
	program = Shader::CreateProgramSimple("shader/line_render_vs.glsl", "shader/line_render_fs.glsl");
	programSolve = Shader::CreateProgramSimple("shader/line_solve_vs.glsl", "shader/line_solve_fs.glsl");
	programCompute = glCreateProgram();
	if (programCompute == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		PAUSE_SHOW_ERROR;
		return 0;
	}
	Shader::AddShaderFromFile(programCompute, "shader/line_compute.glsl", GL_COMPUTE_SHADER);
	Shader::PrepareProgram(programCompute);

	wvpMatLoc = glGetUniformLocation(program, "gWVP");
	worldMatLoc = glGetUniformLocation(program, "gWorld");
	textLoc = glGetUniformLocation(program,"gColorMap");
	vecLoc = glGetUniformLocation(program, "viewVec");
	widthLoc = glGetUniformLocation(program, "width");
	pu.Init(programSolve);
	u_fade.Init(programCompute);
	//OIT realize:
	//Linked List head Texture:
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &headTexture);
	glBindTexture(GL_TEXTURE_2D, headTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, MAX_FRAMEBUFFER_WIDTH, MAX_FRAMEBUFFER_HEIGHT,
		0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindImageTexture(0, headTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
	//headClearBuffer
	glGenBuffers(1, &headClearBuffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, headClearBuffer);
	glBufferData(GL_PIXEL_UNPACK_BUFFER,
		MAX_FRAMEBUFFER_WIDTH*MAX_FRAMEBUFFER_HEIGHT * sizeof(GLuint), NULL, GL_STATIC_DRAW);
	//clear buffer
	GLuint* data = nullptr;
	data = (GLuint*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	memset(data, 0x00, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * sizeof(GLuint));
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

	//atomic counter buffer
	glGenBuffers(1, &atomCounterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomCounterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
	// Create the linked list storage buffer
	linkedList.InitData(GL_RGBA32UI, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * 8 * sizeof(glm::vec4), GL_DYNAMIC_COPY);
	//useless Vertices&Indices
	glGenBuffers(1, &uselessVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uselessVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uselessVertices), uselessVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	glGenBuffers(1, &uselessIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uselessIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uselessIndices), uselessIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

bool Render::LineTech::Update()
{
	return true;
}

void Render::LineTech::Render(Shape::LineStrip & line, Camera& camera, LightComponent& lc)
{
	GLuint * data;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomCounterBuffer);
	data = (GLuint *)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_WRITE_ONLY);
	data[0] = 0;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, headClearBuffer);
	glBindTexture(GL_TEXTURE_2D, headTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//clr_for_alphai.setDefaultData(alphai);
	//alphai.SetData(initPtr);

	glBindImageTexture(0, headTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	linkedList.Bind(1, GL_WRITE_ONLY);
	sqr_g.Bind(2, GL_READ_ONLY);
	alpha.Bind(4);
	alphai.Bind(5);
	
	text.Bind(GL_TEXTURE6);
	//glUniform1i(textLoc, 6);
	//begin to render >>>
	glUseProgram(program);
	//bind uniform data
	glUniformMatrix4fv(wvpMatLoc, 1, GL_TRUE, (const GLfloat*)(&(camera.wvpMat)));
	glUniformMatrix4fv(worldMatLoc, 1, GL_TRUE, (const GLfloat*)(&(camera.worldMat)));
	camera.viewVec = glm::normalize(camera.viewVec);
	glUniform3f(vecLoc,camera.viewVec.x, camera.viewVec.y, camera.viewVec.z);
	glUniform1f(widthLoc, width);

	lc.UpdateData();
	//first pass
	line.Render();
	glBindTexture(GL_TEXTURE_2D, 0);
	//if (programSolve > 0)return;
	glUseProgram(programSolve);

	pu.UpdateData();
	//second pass
	DrawUseLess();
	//compute shader
	glUseProgram(programCompute);
	u_fade.UpdateData();
	alpha.Bind(4);
	alphai.Bind(5);
	alphaTarget.Bind(7);
	glDispatchCompute(segNum /32 , 1,1);
}

void Render::LineTech::Prepare(Texture& texture,Shape::LineStrip & line, Render::Param pfe,float s)
{
	segNum = line.segNum;
	sqr_g.InitData(GL_R32F, segNum * sizeof(float), GL_STATIC_DRAW);
	sqr_g.SetData(line.g.data());

	alpha.InitData(GL_R32F, segNum * sizeof(float), GL_DYNAMIC_COPY);
	alphai.InitData(GL_R32UI, segNum * sizeof(int), GL_DYNAMIC_COPY);
	alphaTarget.InitData(GL_R32F, segNum * sizeof(float), GL_DYNAMIC_COPY);

	float* a;
	a = new float[segNum];
	memset(a, 0, sizeof(float)*segNum);
	alphaTarget.SetData(a);
	delete[] a;

	pu.data = pfe;
	this->s = s;
	text = texture;

	width = line.width;
}

void Render::LineTech::setParams(const float p, const float q, const float r, const float l, const float s)
{
	pu.data.p = p;
	pu.data.q = q;
	pu.data.l = l;
	pu.data.r = r;
	this->s = s;
}

void Render::LineTech::setWidth(const float _width)
{
	width = _width;
}


void Render::LineTech::DrawUseLess()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, uselessVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uselessIBO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
}
