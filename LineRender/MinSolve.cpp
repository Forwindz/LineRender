#include "MinSolve.h"

void Render::ComputeMin::Init(const int _size)
{
	if (size == _size)return;
	size = _size;
	ans.set_size(_size);
	side.set_size(_size);
	cen.set_size(_size);
	temp.set_size(_size);
	h = arma::fmat(_size, _size);
	ans.fill(0.5f);
	needGetH = true;
	computeDone = false;
	//强制转换函数指针
	auto ptr= &ComputeMin::threadMethod;
	Tool::ThreadMethod tm;
	memcpy(&tm, &ptr, sizeof(Tool::ThreadMethod));
	pthread_create(&(thread.th), NULL, Tool::cpy_cast<Tool::ThreadMethod>(&ComputeMin::threadMethod), this);
	thread.Init();
	
}

/*
Here 2D array is not used because H is a sparse matrix,
only about 2 lines of data need to be record
*/
void Render::ComputeMin::Compute(
	const std::vector<int>& _bp, 
	const std::vector<float>& _g)
{
	const int bpSize = _bp.size();

	cen.fill(pfe.p + pfe.s);

	//compute side
	side.set_size(size - 1);
	side.fill(-pfe.s);
	for (int i = 0; i < bpSize; ++i)
	{
		side[_bp[i]] = 0.0f;
		cen[_bp[i]] -= 1.0f*pfe.s;
		cen[_bp[i]-1] -= 0.5f*pfe.s;
		cen[_bp[i]+1] -= 0.5f*pfe.s;
	}
	
	cen[0] -= 0.5f*pfe.s;
	cen[size-1] -= 0.5f*pfe.s;
	//compute cen
#pragma omp parallel for
	for (int i = 0; i < size; ++i)
	{
		temp[i] = pow(1 - _g[i], pfe.l);
		temp[i] *= temp[i];
	}

#pragma omp parallel for
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			float t1 = h.at(i, j)*_g[j] ;
			float t2 = h.at(j, i)*_g[j];
			t1 *= t1;
			t2 *= t2;
			cen[i] += temp[i] * (t1* pfe.q + t2 * pfe.r);
		}
	}
#ifdef DEBUG
	printf(">Compute basic data: done\n");
#endif
}

void Render::ComputeMin::CallThreadCompute(const std::vector<int>* bp, const std::vector<float>* g,const bool _needUpdateH)
{
	if (thread.isRunning())return;
#ifdef DEBUG
	printf("-- call thread to compute --\n");
#endif
	_bp = bp;
	_g = g;
	needGetH = false;
	computeDone = false;
	needUpdateH = _needUpdateH;
	thread.Resume();
}

float * Render::ComputeMin::GetHMatrixPointer()
{
	return h.memptr();
}

float * Render::ComputeMin::GetAns()
{
	return ans.memptr();
}

void Render::ComputeMin::process()
{
	float stepSize = 0.1f;
	const float condition = 0.001f;
	float delta = 0.0f;
	float changed = 0.0f;/*
	for (int i = 0; i < size; i++)
		ans[i] = 1.0f;
	computeDone = true;
	if(ans[2]>0.1f)return;*/
	do
	{
		//compute dx
		//cen*x(i)^2+side*x(i)*x(i+1)-2 p x(i)
		lastC = changed;
		changed = 0.0f;
		for (int i = 1; i < size - 1; ++i)
		{
			temp[i] = 2 * ans[i] * cen[i] + side[i] * (ans[i + 1] + ans[i-1]) - pfe.p*2;
			
		}
		temp[0] = 2 * ans[0] * cen[0] + side[0] * (ans[1]) - pfe.p;
		temp[size - 1] = 2 * ans[size - 1] * cen[size - 1] + side[size - 1] * (ans[size - 2]) - pfe.p;
		
		//begin to change
//#pragma omp parallel for
		for (int i = 0; i < size; ++i)
		{
			float t = ans[i];
			ans[i] -= temp[i] * stepSize;
			if (ans[i] < 0|| ans[i] > 1)
			{
				//ans[i] = 0.0f;
				int temp_int = (int)(ans[i] * 0.5f);
				ans[i] -= temp_int * 2.0f;
				if (ans[i] < 0)ans[i] += 2.0f;
				if (ans[i] > 1.0f)ans[i] = 2.0f - ans[i];
				
			}
			changed += (ans[i]-t)*(ans[i]-t);
			//printf("%f\n", changed);
		}
		//printf("ch=%f\tlch=%f\t >%f\t\n",changed,lastC,stepSize);
		//std::cout << "ch=" << changed << std::endl;
		//if (abs(changed - lastC)/(delta+1.0f) > 1.0f&&delta>0.01f)
		//	stepSize = stepSize * ((delta+1.0f) / abs(changed - lastC));
		if (changed >= lastC && lastC>=condition)
			stepSize *= (lastC / changed)*0.99f;
	} while ((delta=changed) > condition);
#ifdef DEBUG
	printf("> Compute: done\n");
#endif
	computeDone = true;
}

void Render::ComputeMin::threadMethod()
{
	while (1)
	{
		thread.InnerCheck();
		ti.Begin();
		if (needUpdateH)Compute(*_bp, *_g);
		process();
		ti.End();
#ifdef DEBUG
		printf("> cost %d ms\n", ti.GetIntervalMs());
#endif
		thread.Suspend();
	}
}