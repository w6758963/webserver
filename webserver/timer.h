#pragma once
#include"client.h"
#include<sys/time.h>
#include"noncopyable.h"
#include<memory>
#include<queue>
#include<deque>
#include <unistd.h>
#include"MutexLock.hpp"
using namespace::std;
class client;
class timer
{
public:
	timer()
	{

	}
	timer(shared_ptr<client> ptr) {
		clientdata = ptr;
		timeInit();
	}
	~timer()
	{

	}
	shared_ptr<client> clientdata;
	size_t deadline;


	static size_t timeout;
private:

public:
	bool isdead = false;
	bool isLate();
	void timeRefresh();
	void setDead();
	void timeInit();
};
struct timeCmp
{
public:
	bool operator() (shared_ptr<timer>& t1, shared_ptr<timer>& t2)const
	{
		return t1.get()->deadline > t2.get()->deadline;
	}
};
class timerManager
{
private:
	static MutexLock mutex_;
	static priority_queue<shared_ptr<timer>, deque<shared_ptr<timer>>, timeCmp> timerQueue;
public:
	static void clearDeads();
	void addTimer(shared_ptr<client>& t1);
	static void delTimer(weak_ptr<timer> spt);
};

