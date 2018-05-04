#pragma once
#include "PreCompile.h"
namespace Tool
{
	
	class TimeInterval
	{
	public:
		void Begin();
		void End();
		clock_t GetIntervalMs();
	private:
		clock_t begin, end;
	};

	class FPSCalculator
	{
	public:
		void Init();
		void Tick();//Ã¿Ò»Ö¡tickÏÂ
		float GetFPS() const;
		float GetAverageFPS() const;
	private:
		TimeInterval ti;
		float fps;
		float totalTime;
		unsigned long ticks;
		bool first;
	};
}