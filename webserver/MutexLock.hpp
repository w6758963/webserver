#pragma once
#include<mutex>
#include"noncopyable.h"
#include <unistd.h>
#include<sys/syscall.h>
#include <assert.h>
class MutexLock :noncopyable
{
public:

	MutexLock()
	{
		this->holder = 0;
		pthread_mutex_init(&mutex_, NULL);
	}
	~MutexLock() {
		assert(holder == 0);
		pthread_mutex_destroy(&mutex_);
	}
	void lock() {
		pthread_mutex_lock(&mutex_);
		holder = syscall(SYS_gettid);
	}
	void unlock() {
		holder = 0;
		pthread_mutex_unlock(&mutex_);
	}
	pthread_mutex_t* getPthreadMutex()
	{
		return &mutex_;
	}
private:
	pthread_mutex_t mutex_;
	pid_t holder;
};
class MutexLockGuard :noncopyable
{
public:
	MutexLockGuard(MutexLock& mutex) :mutex_(mutex)
	{
		mutex_.lock();
	}
	~MutexLockGuard() {
		mutex_.unlock();
	}
private:
	MutexLock& mutex_;

};
