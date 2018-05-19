#include "LineTech.h"
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
Render::LineTech::~LineTech()
{
	glDeleteTextures(1, &linkedListTexture);
	glDeleteTextures(1, &headTexture);
	glDeleteTextures(1, &matrixHBuffer);
	glDeleteTextures(1, &alphaListTexture);

	glDeleteBuffers(1, &headClearBuffer);
	glDeleteBuffers(1, &hClearBuffer);
	glDeleteBuffers(1, &atomCounterBuffer);
	glDeleteBuffers(1, &linkedListBuffer);
	glDeleteBuffers(1, &alphaListBuffer);
	glDeleteBuffers(1, &uselessVBO);
	glDeleteBuffers(1, &uselessIBO);

	glDeleteProgram(solveHProgram);
	glDeleteProgram(solveNoHProgram);

}

bool Render::LineTech::Init()
{
	//shader for rendering
	program = Shader::CreateProgramSimple("shader/line_render_vs.glsl", "shader/line_render_fs.glsl");
	wvpMatLoc = glGetUniformLocation(program, "gWVP");
	worldMatLoc = glGetUniformLocation(program, "gWorld");

	solveHProgram = glCreateProgram();
	solveNoHProgram = glCreateProgram();
	GLuint sobjv = Shader::AddShaderFromFile(solveHProgram, "shader/line_solve_vs.glsl", GL_VERTEX_SHADER);
	glAttachShader(solveNoHProgram, sobjv);
	Shader::AddShaderFromFile(solveHProgram, "shader/line_solve_with_H_fs.glsl", GL_FRAGMENT_SHADER);
	Shader::AddShaderFromFile(solveNoHProgram, "shader/line_solve_without_H_fs.glsl", GL_FRAGMENT_SHADER);

	Shader::PrepareProgram(solveHProgram);
	Shader::PrepareProgram(solveNoHProgram);

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
	glGenBuffers(1, &linkedListBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, linkedListBuffer);
	glBufferData(GL_TEXTURE_BUFFER, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * 3 * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	//push the buffer into linked list
	glGenTextures(1, &linkedListTexture);
	glBindTexture(GL_TEXTURE_BUFFER, linkedListTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32UI, linkedListBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	glBindImageTexture(1, linkedListTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);

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

	//clear h
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, hClearBuffer);
	glBindTexture(GL_TEXTURE_2D, matrixHBuffer);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, segNum, segNum, GL_RED, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomCounterBuffer);
	data = (GLuint *)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_WRITE_ONLY);
	data[0] = 0;
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, headClearBuffer);
	glBindTexture(GL_TEXTURE_2D, headTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindImageTexture(0, headTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(1, linkedListTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);
	glBindImageTexture(2, matrixHBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(3, alphaListTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

	//begin to render >>>

	glUseProgram(program);

	//bind uniform data
	glUniformMatrix4fv(wvpMatLoc, 1, GL_TRUE, (const GLfloat*)(&(camera.wvpMat)));
	glUniformMatrix4fv(worldMatLoc, 1, GL_TRUE, (const GLfloat*)(&(camera.worldMat)));
	lc.UpdateData();
	
	line.Render();//first time to render, save all fragments

	bool tneed;
	if (tneed = camera.HasUpdated()&&computeAlpha.needGetH)
	{
		glUseProgram(solveHProgram);
	}
	else
	{
		glUseProgram(solveNoHProgram);
	}

	//transport data
	if (computeAlpha.computeDone)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, alphaListBuffer);
		float* alphaPointer = (float*)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
		memcpy(alphaPointer, computeAlpha.GetAns(), sizeof(float)*segNum);
		glUnmapBuffer(GL_TEXTURE_BUFFER);
		computeAlpha.needGetH = true;
		computeAlpha.computeDone = false;
	}

	DrawUseLess();

	if (tneed)
	{
		glBindTexture(GL_TEXTURE_2D, matrixHBuffer);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, (char*)computeAlpha.GetHMatrixPointer());
		computeAlpha.CallThreadCompute(&(line.gapRecord), &(line.g), tneed);
	}
}

void Render::LineTech::Prepare(Shape::LineStrip & line, Render::ComputeMin::ParamForE pfe)
{
	segNum = line.segNum;

	glDeleteBuffers(1, &alphaListBuffer);
	glDeleteTextures(1, &alphaListTexture);
	glGenBuffers(1, &alphaListBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, alphaListBuffer);
	glBufferData(GL_TEXTURE_BUFFER, line.segNum * sizeof(float), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glGenTextures(1, &alphaListTexture);
	glBindTexture(GL_TEXTURE_BUFFER, alphaListTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, alphaListBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	//h matrix clear
	glDeleteBuffers(1, &hClearBuffer);
	glDeleteTextures(1, &matrixHBuffer);
	glGenBuffers(1, &hClearBuffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, hClearBuffer);
	glBufferData(GL_PIXEL_UNPACK_BUFFER,
		segNum * segNum * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	//clear buffer
	GLuint* data2 = nullptr;
	data2 = (GLuint*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	memset(data2, 0x00, segNum * segNum * sizeof(GLfloat));
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

	glGenTextures(1, &matrixHBuffer);
	glBindTexture(GL_TEXTURE_2D, matrixHBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, segNum, segNum,
		0, GL_RED, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	computeAlpha.Init(segNum);
	computeAlpha.pfe = pfe;
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
