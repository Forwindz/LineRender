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
	
	for (int i = 0; i < size; ++i)
	{
		temp[i] = pow(1 - _g[i], pfe.l);
	}

	//cost a lot of time computing this thing...
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			cen[i] += temp[i] * (h.at(i, j)*_g[j] * pfe.q + h.at(j, i)*_g[j] * pfe.r);
		}
	}
#ifdef DEBUG
	printf("compute basic data done\n");
#endif
}

void Render::ComputeMin::CallThreadCompute(const std::vector<int>* bp, const std::vector<float>* g,const bool _needUpdateH)
{
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
	const float stepSize = 0.15f;
	const float condition = stepSize*0.001f;
	float change=0.0f;
	float changed = 0.0f;
	do
	{
		//compute dx
		change = 0;
		for (int i = 1; i < size - 1; ++i)
		{
			temp[i] = 2 * ans[i] * cen[i] + side[i] * (ans[i + 1] + ans[i - 1]) - pfe.p;
			change += abs(temp[i]);
		}
		temp[0] = 2 * ans[0] * cen[0] + side[0] * (ans[1]) - pfe.p;
		temp[size - 1] = 2 * ans[size - 1] * cen[size - 1] + side[size - 1] * (ans[size - 2]) - pfe.p;
		//change += abs(temp[0] + temp[size - 1]);
		

		
		//begin to change
		float t;
		for (int i = 0; i < size; ++i)
		{
			t = ans[i];
			ans[i] -= temp[i] * stepSize;
			if (ans[i] < 0|| ans[i] > 1)
			{
				ans[i] = 0.0f;
				int temp_int = (int)(ans[i] * 0.5f);
				ans[i] = abs(ans[i] - temp_int * 2.0f - 1.0f);
			}
			changed = (t - ans[i])*(t - ans[i]);
		}
		std::cout << ">>\t" << change << " || \t"
			<< abs(lastC - change) << "\t @" << condition << std::endl;
		changed = change - lastC;
		lastC = change;
		if (abs(changed) <= condition)break;
	} while (1);
	/*
	std::cout << "\n\n=====================\n\n";
	for (int i = 0; i < size/25; ++i)
	{
		std::cout << ans[i] << "\t";
		if (i % 8 == 0)std::cout << "\n";
	}
	int co = 0;
	for (int i = 0; i < size; ++i)
	{
		if (abs(ans[i]-0.5f)<=0.2f)++co;
	}
	std::cout << "\n>>>co="<<co << "\n";
	system("pause");*/
#ifdef DEBUG
	printf("down done\n");
#endif
	computeDone = true;
}

void Render::ComputeMin::threadMethod()
{
	while (1)
	{
		//printf("thread> Inner\n");
		thread.InnerCheck();
		//printf("thread> Compute\n");
		if (needUpdateH)Compute(*_bp, *_g);
		process();
		thread.Suspend();
	}
}