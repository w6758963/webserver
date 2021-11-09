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
	CHECK_STATE_REQUESTLINE = 0,	//此时正在分析请求行
	CHECK_STATE_HEADER,				//此时正在分析头部字段
	CHECK_STATE_CONTENT				//内容
};
enum LINE_STATUS
{
	LINE_OK = 0,			//读到了完整的行
	LINE_BAD,				//读取行错误
	LINE_LOADING			//读取的行不完整，还需要继续输入
};
enum HTTP_CODE
{
	NO_REQUEST,				//请求不完整，需要继续读入行
	GET_REQUEST,			//获得了完整的请求
	BAD_REQUEST,			//活的请求的语法错误
	FILE_REQUEST,			//文件已被请求
	FORBIDDEN_REQUEST,		//客户访问权限不足
	INTERNAL_ERROR,			//服务器出错
	CLOSED_CONNECTION		//客户端已经关闭了连接
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
	int clientfd;		//对应sockfd
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
	HTTP_CODE request_line_parse();		//请求行解析
	HTTP_CODE headers_parse();			//头部解析
	HTTP_CODE requestResponse();		//对获得的请求进行反馈

	void add_status_line(int code, string buff);	//写入状态行
	void add_header(int code);						//首部字段
	void add_content(string buff);			//内容
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

