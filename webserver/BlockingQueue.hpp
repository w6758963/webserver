#pragma once
#include"condition.hpp"
#include"queue"
#include"MutexLock.hpp"
template<typename T>
class BlockingQueue
{
public:
	BlockingQueue() :mutex_(), queue_(), notEmpty(mutex_) {}
	T take();
	void put(const T& taskfunc)
	{
		MutexLockGuard lock(mutex_);
		queue_.push_back(taskfunc);
		notEmpty.notify();
	}
private:
	mutable MutexLock mutex_;
	std::deque<T>     queue_;
	condition         notEmpty;
};


template<typename T>
inline T BlockingQueue<T>::take()
{
	MutexLockGuard lock(mutex_);
	while (queue_.empty())
	{
		notEmpty.wait();
	}
	assert(!queue_.empty());
	T front(queue_.front());
	queue_.pop_front();
	return front;
}

