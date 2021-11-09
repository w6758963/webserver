#include "threadPool.h"
BlockingQueue<Task> threadPool::taskQueue;
vector<pthread_t> threadPool::workers;
int threadPool::shutdown = 0;
void threadPool::taskAppend(Task user)
{
	taskQueue.put(user);
	return;
}
void* threadPool::workerThread(void* data)
{
	while (1)
	{
		Task task = taskQueue.take();
		shared_ptr<client> sp_client = static_pointer_cast<client>(task);
		sp_client->process();
		if (shutdown == 1)
		{
			return NULL;
		}
		
	}

}
int threadPool::initThreadPool(int poolsnum)
{
	for (int i = 0; i < poolsnum; i++)
	{
		pthread_t tid_;
		int error = pthread_create(&tid_, NULL, workerThread, NULL);
		workers.push_back(tid_);
	}
	return 0;
}



