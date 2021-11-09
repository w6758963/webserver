#pragma once
#include<iostream>
using namespace::std;

#include"BlockingQueue.hpp"
class logbuff
{
public:
	logbuff() { maxsize = 1000; };
	logbuff(int size)
	{
		maxsize = size;
	}

private:
	string logBuffer;
	int maxsize;
};

class mylog
{
public:
	mylog(string loc, int logbuffSize_ = 1000)	
	{
		logfileLoc = loc;
		logbuffSize = logbuffSize_;
		logQueue.put(logbuff(logbuffSize));
	}
	

private:
	logbuff writebuff;
	logbuff readbuff;

	int logbuffSize;
	BlockingQueue<logbuff> logQueue;
	string logfileLoc;
};