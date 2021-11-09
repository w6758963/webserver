#pragma once
#include<pthread.h>
#include"BlockingQueue.hpp"
#include"client.h"
using namespace::std;
typedef shared_ptr<void> Task;
class threadPool
{
public:
	static BlockingQueue<Task> taskQueue;
private:
	static int shutdown;
	static vector<pthread_t> workers;
public:
	static int initThreadPool(int poolsnum);
	static void taskAppend(Task user);
	static void* workerThread(void* data);
};

