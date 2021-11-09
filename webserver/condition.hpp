#pragma once
#include"condition_variable"
#include"MutexLock.hpp"
#include"noncopyable.h"

class condition :noncopyable
{
public:
	condition(MutexLock& mutex) :mutex_(mutex)
	{
		pthread_cond_init(&cond_, NULL);
	}
	~condition()
	{
		pthread_cond_destroy(&cond_);
	}
	void wait()
	{
		pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
	}
	void notify()
	{
		pthread_cond_signal(&cond_);
	}
	void notifyAll()
	{
		pthread_cond_broadcast(&cond_);
	}
private:
	MutexLock& mutex_;
	pthread_cond_t cond_;

};