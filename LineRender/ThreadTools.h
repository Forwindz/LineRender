#pragma once
#include "PreCompile.h"
namespace Tool
{
	typedef void*(__cdecl *ThreadMethod)(void*);

	class PThread
	{
	public:
		~PThread();
		void Init();
		void Suspend();
		void Resume();
		void InnerCheck();
		pthread_t th;
	private:
		pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t cond;
		int flag = 0;
	};
}