#include "timer.h"
size_t timer::timeout = 500;
MutexLock timerManager::mutex_;
priority_queue<shared_ptr<timer>, deque<shared_ptr<timer>>, timeCmp> timerManager::timerQueue;
bool timer::isLate()
{
	if (this->isdead == true)
		return true;
	struct timeval now;
	gettimeofday(&now, NULL);
	size_t now_ = now.tv_sec * 1000 + now.tv_usec / 1000;
	if (now_ >= deadline)
	{
		isdead = true;
		return true;
	}
	else
		isdead = false;
	return false;
}
void timer::timeRefresh()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	deadline = now.tv_sec * 1000 + now.tv_usec / 1000 + timeout;
}

void timer::setDead()
{
	
	isdead = true;
}

void timer::timeInit()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	deadline = now.tv_sec * 1000 + now.tv_usec / 1000 + timeout;
}



void timerManager::clearDeads()
{
	MutexLockGuard locker(mutex_);
	while (!timerQueue.empty())
	{
		shared_ptr<timer> tem = timerQueue.top();
		if (tem->isLate() == true)
		{
			tem->setDead();
			tem->clientdata->closeConnect();
			tem->clientdata.reset();
			tem.reset();
			timerQueue.pop();
		}
		else
			break;
	}
}

void timerManager::addTimer(shared_ptr<client>& t1)
{
	shared_ptr<timer> newtimer = make_shared<timer>(timer(t1));
	timerQueue.push(newtimer);
	t1->linktimer(newtimer);
}

void timerManager::delTimer(weak_ptr<timer> spt)
{
	if (spt.use_count()!=0)
	{
		shared_ptr<timer> temp = spt.lock();
		temp->setDead();
		timerManager::clearDeads();
	}
}
