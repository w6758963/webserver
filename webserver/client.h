#pragma once
#include"epoll.h"
#include"tool.h"
#include"threadPool.h"
#include"timer.h"

#include<sys/epoll.h>
#include<unistd.h>
#include <errno.h>
#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<memory>
#include<string>
#include<map>
using namespace::std;
class timer;
enum CHECK_STATE
{
	CHECK_STATE_REQUESTLINE = 0,	//��ʱ���ڷ���������
	CHECK_STATE_HEADER,				//��ʱ���ڷ���ͷ���ֶ�
	CHECK_STATE_CONTENT				//����
};
enum LINE_STATUS
{
	LINE_OK = 0,			//��������������
	LINE_BAD,				//��ȡ�д���
	LINE_LOADING			//��ȡ���в�����������Ҫ��������
};
enum HTTP_CODE
{
	NO_REQUEST,				//������������Ҫ����������
	GET_REQUEST,			//���������������
	BAD_REQUEST,			//���������﷨����
	FILE_REQUEST,			//�ļ��ѱ�����
	FORBIDDEN_REQUEST,		//�ͻ�����Ȩ�޲���
	INTERNAL_ERROR,			//����������
	CLOSED_CONNECTION		//�ͻ����Ѿ��ر�������
};
enum METHOD
{
	GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNCT, PATCH
};
class client
{
public:
	
	client()
	{

	}
	client(int fd)
	{
		clientfd = fd;

	}
	client(int fd , struct sockaddr_in caddr)
	{
		clientAddr = caddr;
		clientfd = fd;
	}
	~client()
	{
		//closeConnect();
	}
	//typedef std::shared_ptr<client> sharedptr_client; 	
	static int epollfd;
	static int hostfd;
	static std::shared_ptr<client> clients[1024];


	std::weak_ptr<timer> clientTimer;
	int clientfd;		//��Ӧsockfd
	struct sockaddr_in clientAddr;
	string readbuff;
	string writebuff;
	CHECK_STATE checkState = CHECK_STATE_REQUESTLINE;
	METHOD 	method = GET;
	string urlstr;
	string version;
	map<string, string> header;
	bool isKeepAlive = false;
	string path;

	//int read_idx = 0;
	
	bool forWrite = false;
	bool forRead = false;
public:

	int closeConnect();
	bool write();
	bool read(); 
	int process();
	bool isforRead();
	bool isforWrite();
	void setRead();
	void setWrite();
	HTTP_CODE process_read();
	bool process_write(HTTP_CODE flag);
	HTTP_CODE request_line_parse();		//�����н���
	HTTP_CODE headers_parse();			//ͷ������
	HTTP_CODE requestResponse();		//�Ի�õ�������з���

	void add_status_line(int code, string buff);	//д��״̬��
	void add_header(int code);						//�ײ��ֶ�
	void add_content(string buff);			//����
	void add_content_len(int content_len);
	void add_connection();
	void add_blank();
	bool stringcmp(string s1,string s2);
	int get_space(string s,int begin,int end);
	int get_url(int endflag);

	int linktimer(shared_ptr<timer>t);
	static int acceptNewConnect(int hostfd);
	static void initClient(int hostfd_, int epollfd_);
	static int clientDistribute(int eventnum , epoll_event* events_);


};

