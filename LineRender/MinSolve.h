#pragma once
#include "PreCompile.h"
#include "SimpleTools.h"
#include "ThreadTools.h"
#include "TimeTool.h"

//This class is specificed for computing the alpha of segments
namespace Render
{
	class ComputeMin
	{
	public:

		struct ParamForE
		{
			float p, q, r, s, l;
		};

		void Init(const int _size);

		//return ans(alpha), input segCount,bp,g
		void Compute(
			const std::vector<int>& bp,
			const std::vector<float>& g);
		void CallThreadCompute(const std::vector<int>* bp, const std::vector<float>* g,const bool needUpdateH);

		float* GetHMatrixPointer();
		float* GetAns();

		ParamForE pfe;
		volatile bool needGetH;
		volatile bool computeDone;
		
	private:
		/*
		Matrix Q: size*size

		cen[0]    side[0]    0           ...0 0 0 
		side[0]   cen[1]     side[1]  ...
		0         side[1]    cen[2]   ...
		.                      .
		.                        .
		.                           .


		final:
		cen*x^2+side*xi*xj+side*xj*xi+pfe.p*0.5*x
		*/
		arma::fvec ans,side,cen,temp;
		arma::fmat h;
		//temp var for compute
		const std::vector<int>* _bp;
		const std::vector<float>* _g;
		float lastC;

		int size = -1;

		bool needUpdateH = true;

		void process();
		void threadMethod();
		
		Tool::PThread thread;
		Tool::TimeInterval ti;
	};
	
}