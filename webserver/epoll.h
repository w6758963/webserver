#pragma once
#include<sys/epoll.h>
#include<unistd.h>
#include <errno.h>
#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"client.h"
#include"tool.h"
//#include"timer.h"
class client;
class timerManager;
class epoll
{
public:
	static int epollfd;
	static epoll_event events[1024];
	static int hostFd;
	static timerManager timer_manager;
public:
	static int mod_epoll(int clientfd,  int ev,int epfd = epoll::epollfd);
	static int del_epoll(int clientfd,int epfd = epoll::epollfd);
	static int add_epoll(int clientfd);
	static void epollwait(int maxevents, int tiemout);
	static int epollinit(int hostfd, int size);
};

