#include "TimeTool.h"

void Tool::TimeInterval::Begin()
{
	end = begin = clock();
}

void Tool::TimeInterval::End()
{
	end = clock();
}

clock_t Tool::TimeInterval::GetIntervalMs()
{
	return end - begin;
}

void Tool::FPSCalculator::Init()
{
	ti.Begin();
	ticks = 0;
	totalTime = 0.0f;
	first = true;
}

void Tool::FPSCalculator::Tick()
{
	ti.End();
	++ticks;
	if (first)
	{
		totalTime += ti.GetIntervalMs();
		first = false;
	}
	fps = 1000.0f / ti.GetIntervalMs();
	ti.Begin();
}

float Tool::FPSCalculator::GetFPS() const
{
	return fps;
}

float Tool::FPSCalculator::GetAverageFPS() const
{
	return 1000.0f*ticks/totalTime;
}
