#include "PreCompile.h"
#include "Tech.h"
#include "Line.h"
#include "Camera.h"
#include "Light.h"
#include "ListTexture.h"
#include "hugeVector.h"
#include "SimpleTools.h"
#include "Params.h"
#include "Clear1DTexture.h"
#include "Texture.h"
#include "UniformStruct.h"

namespace Render
{
	const static unsigned int MAX_FRAMEBUFFER_WIDTH = SCREEN_WIDTH;
	const static unsigned int MAX_FRAMEBUFFER_HEIGHT = SCREEN_HEIGHT;

	struct FadeParam {
		float s;
		float it_times;
		float fade_speed;
	};

	class LineTech :public Tech
	{
	public:
		LineTech();
		virtual ~LineTech();
		virtual bool Init();
		virtual bool Update();
		void Render(Shape::LineStrip& line, Camera& camera, LightComponent& lc);
		void Prepare(Texture& texture,Shape::LineStrip& line, Render::Param pfe,float s);
		void setParams(const float p, const float q, const float r, const float l, const float s);
		void setWidth(const float _width);
		void Release();

		Render::ParamUniform pu;
		UniformStruct<FadeParam> u_fade;
	protected:
		void DrawUseLess();
		GLuint headTexture,
			atomCounterBuffer,
			headClearBuffer,
			programSolve, programCompute,
			uselessVBO, uselessIBO,
			wvpMatLoc, worldMatLoc, textLoc, widthLoc, vecLoc;
		ListTexture sqr_g, alpha,alphai, linkedList, alphaTarget;
		
		//Clear1DTexture clr_for_alphai;
		int segNum = 0;
		float s = 0.0f;
		float width = 0.1f;
		unsigned int* initPtr = nullptr;
		Texture text;
	};
}