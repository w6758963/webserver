#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<pthread.h>
using namespace::std;
#include"tool.h"
#include"epoll.h"
#include"client.h"
#include"threadPool.h"
#define LISTENMAX	1024	//socket��������
#define EPOLLMAX	1024	//epoll����¼�����
#define POOLSIZE	8		//�̳߳ش�С


int setHostSock(int port);
int main()
{

	//�����̳߳�
	threadPool::initThreadPool(POOLSIZE);

	//����������socket����
	int hostfd = setHostSock(10086);
	if (hostfd == -1)
	{
		printf("error1");
		return -1;
	}
	//��socket����Ϊ������
	setNonBlocking(hostfd);
	

	//��ʼ��epoll

	int epollfd = epoll::epollinit(hostfd, EPOLLMAX);
	if (epollfd == -1)
	{
		printf("error2");
		close(hostfd);
		close(epollfd);
		return -1;
	}
	client::initClient(hostfd, epollfd);
	//oneloop
	
	while(1)
	{
		epoll::epollwait(1000,-1);
	}

	cout << "ok" << endl;
	return 0;
}


int setHostSock(int port)
{
	//���˿��Ƿ����
	if (port > 65535 || port < 1024)
	{
		printf("port error");
		return -1;
	}
	//��������sock��IPv4+TCP��
	int hostfd = 0;
	if ((hostfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket build error");
		return -1;
	}
	//��ֹ��ַ�ظ���
	int optval = 1;
	if (setsockopt(hostfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		printf("socket build reply error");
		return -1;
	}
	struct sockaddr_in serverAddr;
	memset((char*)&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons((unsigned short)port);
	if (bind(hostfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
		printf("serverAddr bind error");
		return -1;
	}
	if (listen(hostfd, LISTENMAX) == -1)
	{
		printf("serverSOCK listen error");
		return -1;
	}
	if (hostfd == -1)
	{
		printf("hostfd=-1 error");
		close(hostfd);
		return -1;
	}
	return hostfd;
}

string numtostr(int num)
{
	string re1;
	while (1)
	{
		int t = num % 10;
		re1.push_back(t + 48);
		num = num / 10;
		if (num == 0)
			break;
	}
	string re;
	for (int i = re1.size() - 1; i >= 0; i--)
	{
		re.push_back(re1[i]);
	}
	return re;
}
