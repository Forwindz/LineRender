#include "ThreadTools.h"

Tool::PThread::~PThread()
{
	pthread_detach(th);
}

void Tool::PThread::Init()
{
	flag = 0;
	mut = PTHREAD_MUTEX_INITIALIZER;
	cond = NULL;
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
}

void Tool::PThread::Suspend()
{
	pthread_mutex_lock(&mut);
	flag--;
	pthread_mutex_unlock(&mut);
}

void Tool::PThread::Resume()
{
	pthread_mutex_lock(&mut);
	flag++;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mut);
}

void Tool::PThread::InnerCheck()
{
	pthread_mutex_lock(&mut);
	while (flag <= 0)
	{
		pthread_cond_wait(&cond, &mut);
	}
	pthread_mutex_unlock(&mut);
}
