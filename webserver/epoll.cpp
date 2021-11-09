#include "epoll.h"
int epoll::epollfd = 0;
epoll_event epoll::events[1024];
int epoll::hostFd;


int epoll::mod_epoll(int clientfd, int ev, int epfd)
{
	epoll_event event_;
	event_.data.fd = clientfd;
	event_.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
	epoll_ctl(epfd,EPOLL_CTL_MOD,clientfd, &event_);
	return 0;
}

int epoll::del_epoll(int clientfd, int epfd)
{
	epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, 0);
	close(clientfd);
	client::clients[clientfd].reset();
	return 0;
}

int epoll::add_epoll(int clientfd)
{
	epoll_event ev;
	ev.data.fd = clientfd;
	ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	int re = epoll_ctl(epoll::epollfd, EPOLL_CTL_ADD, clientfd, &ev);
	setNonBlocking(clientfd);
	return 0;
}

void epoll::epollwait(int maxevents,int tiemout)
{
	//����epoll����¼���
	if (maxevents >= 1024)
		maxevents = 1023;
	//epoll�¼��ȴ�
	//close(4);
	//int re = fcntl(3, F_GETFD, 0);
	int eventcount = epoll_wait(epollfd, events, maxevents, -1);
	//eventcount��ÿ�δ������¼�����
	//����client�¼�������

	client::clientDistribute(eventcount, events);


}
int epoll::epollinit(int hostfd ,int size)
{
	epoll::epollfd = epoll_create(size);
	if (epollfd == -1)
	{
		printf("epollfd create fail");
		return -1;
	}
	epoll_event ev;
	ev.data.fd = hostfd;
	//ET����
	ev.events = EPOLLIN | EPOLLET;
	int re = epoll_ctl(epollfd, EPOLL_CTL_ADD, hostfd, &ev);
	if (re == -1)
	{
		printf("epoll add fail");
		close(epollfd);
		return -1;
	}
	epoll::hostFd = hostfd;
	return epollfd;

}
