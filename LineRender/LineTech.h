#include "PreCompile.h"
#include "Tech.h"
#include "Line.h"
#include "Camera.h"
#include "MinSolve.h"
#include "Light.h"

namespace Render
{
	const static unsigned int MAX_FRAMEBUFFER_WIDTH = SCREEN_WIDTH;
	const static unsigned int MAX_FRAMEBUFFER_HEIGHT = SCREEN_HEIGHT;

	//TODO:Camera param support
	class LineTech :public Tech
	{
	public:
		virtual ~LineTech();
		virtual bool Init();
		virtual bool Update();
		void Render(Shape::LineStrip& line, Camera& camera, LightComponent& lc);
		void Prepare(Shape::LineStrip& line, Render::ComputeMin::ParamForE pfe);
	protected:
		void DrawUseLess();
		GLuint headTexture, linkedListTexture, alphaListTexture,
			linkedListBuffer, atomCounterBuffer, matrixHBuffer, alphaListBuffer,
			headClearBuffer, hClearBuffer,
			solveHProgram, solveNoHProgram,
			uselessVBO, uselessIBO,
			wvpMatLoc, worldMatLoc;
		Render::ComputeMin computeAlpha;
		int segNum = 0;
	};
}