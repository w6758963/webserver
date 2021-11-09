#include "client.h"
int client::hostfd;
int client::epollfd;
std::shared_ptr<client> client::clients[1024];
timerManager epoll::timer_manager;
string ok_200_title("OK");

string root_doc("/home/whd/webserver");
//string test("<html><body>hello<body><html>");
string test1("<html><body>");
string test2("<body><html>");

enum process_mode
{
    READMODE = 0,
    WRITEMODE = 1 
};

int client::closeConnect()
{
    epoll::del_epoll(this->clientfd);
    return 0;
}
bool client::write()
{
    send(clientfd, writebuff.c_str(), writebuff.size(), 0);
    return false;
}
bool client::read()
{
    char* recvbuff = new char[2048];
    while (1)
    {
        int re = recv(clientfd, recvbuff, 2048, 0);
        if (re == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            return false;
        }
        else if (re == 0)
        {
            return false;
        }
        readbuff.append(recvbuff,re);
        memset(recvbuff, 0, 2048);
    }
    delete[] recvbuff;
    return true;
}
int client::process()
{
    process_mode MODE;
    if (isforRead())
        MODE = READMODE;
    else if (isforWrite())
        MODE = WRITEMODE;
    switch (MODE)
    {
    case READMODE: {
        bool readresult = read();

        if (readresult == false)
        {
            //如果读入失败，就关闭socket，注销事件
            epoll::del_epoll(this->clientfd);
            this->clientTimer.lock()->setDead();
            return -1;
        }
        HTTP_CODE re_read = process_read();
        if (re_read == NO_REQUEST)
        {
            epoll::mod_epoll(this->clientfd, EPOLLIN);
            return 0;
        }
        process_write(re_read);
        epoll::mod_epoll(clientfd, EPOLLOUT);
        break;
    }
    case WRITEMODE: {
        write();
        break;
    }
    default:
        break;
    }
}
bool client::isforRead()
{
    bool re = forRead;
    if (forRead == true)
        forRead = false;
    return re;
}
bool client::isforWrite()
{
    bool re = forWrite;
    if (forWrite == true)
        forWrite = false;
    return re;
}
void client::setRead()
{
    forRead = true;
}
void client::setWrite()
{
    forWrite = true;

}
HTTP_CODE client::process_read()
{
    
    //获取一行的长度,如果小于0,说明读取不完整
    HTTP_CODE re = NO_REQUEST;
    while (re!= GET_REQUEST)
    {
        switch (checkState)
        {
        case CHECK_STATE_REQUESTLINE: {
            re = request_line_parse();
            if (re == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            break;
        }
        case CHECK_STATE_HEADER: {
            re = headers_parse();
            if (re == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            else if (re == GET_REQUEST)
            {
                return requestResponse();
            }
            break;
        }
        default:
            break;
        }
    }
    

}
HTTP_CODE client::request_line_parse()
{
    int endflag = 0;
    for (; endflag < readbuff.size(); endflag++)
    {
        if (endflag == readbuff.size() - 1)
            return BAD_REQUEST;
        if (readbuff[endflag] == '\r' && readbuff[endflag + 1] == '\n')
            break;
    }
    endflag++;
    string requestRow = readbuff.substr(0, endflag - 1);
    if (stringcmp(requestRow, "GET"))
        method = GET;
    else {
        return BAD_REQUEST;
    }
    int urled = get_url(endflag);
    if(urled <0)
        return BAD_REQUEST;
    int ver = urled;
    for (; ver <= endflag; ver++)
    {
        if (ver == endflag)
        {
            ver = -1;
            break;
        }
        if (readbuff[ver + 1] == '\r')
            break;
    }
    if (ver <= 0)
        return BAD_REQUEST;
    version = readbuff.substr(urled, ver + 1 - urled);
    if (stringcmp(version, "HTTP/1.1") != true)
    {
        return BAD_REQUEST;
    }
    if (stringcmp(urlstr, "http://") == true)
    {
        urlstr = urlstr.substr(6);
    }
    if (urlstr.size() == 0 || urlstr[0] != '/')
        return BAD_REQUEST;
    checkState = CHECK_STATE_HEADER;
    return NO_REQUEST;
}
HTTP_CODE client::headers_parse()
{
    checkState = CHECK_STATE_HEADER;
    return GET_REQUEST;
}
HTTP_CODE client::requestResponse()
{
    return GET_REQUEST;
    string realflie = root_doc + urlstr;
    int resfd = open(realflie.c_str(), O_RDONLY);
    return FILE_REQUEST;
}
void client::add_status_line(int code, string buff)
{
    writebuff.append(version+" ");
    writebuff.append( numtostr(code) + " ");
    writebuff.append(buff + "\r\n");
}
void client::add_header(int code)
{
    add_content_len(code);
    add_connection();
    add_blank();
}
void client::add_content(string buff)
{
    writebuff.append(buff);
}
void client::add_content_len(int content_len)
{
    writebuff.append("Content_length: " + numtostr(content_len) + "\r\n");

}
void client::add_connection()
{
    writebuff.append("Connection: ");
    if (isKeepAlive == false)
        writebuff.append("close");
    else
        writebuff.append("keep-alive");
    writebuff.append("\r\n");
    return;
}
void client::add_blank()
{
    //timer t;
    writebuff.append("\r\n");
}


bool client::process_write(HTTP_CODE flag)
{
    switch (flag)
    {
    case NO_REQUEST:
        break;
    case GET_REQUEST:
    {
        add_status_line(200, ok_200_title);

        string tem = test1;
        tem.push_back(48 + this->clientfd);
        tem.append(test2);

        add_header(tem.size());
        add_content(tem);
        break;
    }
    case BAD_REQUEST:
        break;
    case FILE_REQUEST:
        break;
    case FORBIDDEN_REQUEST:
        break;
    case INTERNAL_ERROR:
        break;
    case CLOSED_CONNECTION:
        break;
    default:
        break;
    }
}

bool client::stringcmp(string s1, string s2)
{
    int l1 = s1.size(), l2 = s2.size();
    for (int i = 0; i < l1; i++)
    {
        if (s1[i] >= 65 && s1[i] <= 90) s1[i] = tolower(s1[i]);
    }
    for (int i = 0; i < l2; i++)
    {
        if (s2[i] >= 65 && s2[i] <= 90) s2[i] = tolower(s2[i]);
    }
    int re = s1.find(s2);
    if (re >= 0)
        return true;
    return false;
}
int client::get_space(string s,int begin,int end)
{
    for (int i = begin; i <= end; i++)
    {
        if (i == end)
            return -1;
        if (s[i] == ' ')
            return i;
    }
    return -1;
}
int client::get_url(int endflag)
{
    int url = get_space(readbuff, 0, endflag);
    url++;
    if (url == 0)
        return -1;
    int urlend = get_space(readbuff, url, endflag);
    urlend--;
    if (urlend < 0)
        return -1;
    urlstr = readbuff.substr(url, urlend + 1 - url);
    return urlend+2;
}
client s1;

int client::linktimer(shared_ptr<timer> t)
{
    clientTimer = t;
    return 0;
}
int client::acceptNewConnect(int hostfd)
{
	struct sockaddr_in clientaddr;
	socklen_t clientLen = sizeof(clientaddr);
    while (1)
    {
        int connfd = accept(epoll::hostFd, (sockaddr*)&clientaddr, &clientLen);
        if (connfd <= 3)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return 0;
            printf("connfd accept error");
            return -1;
        }
        setNonBlocking(connfd);
        //printf("connfd conm from %s", inet_ntoa(clientaddr.sin_addr));
        epoll::add_epoll(connfd);
        client::clients[connfd] = std::make_shared<client>(client(connfd, clientaddr));
        //绑定定时器
        epoll::timer_manager.addTimer(client::clients[connfd]);
        //epoll::timer_manager.addTimer(epoll::clients[connfd]);
    }
	
    return 0;
}
void client::initClient(int hostfd_, int epollfd_)
{
    client::hostfd = hostfd_;
    client::epollfd = epollfd_;
    client::clients[hostfd] = make_shared<client>(client(hostfd));
    return;
}
int client::clientDistribute(int eventnum, epoll_event* events_)
{

    for (int i = 0; i < eventnum; i++)
    {
        int newfd = events_[i].data.fd;
        if (newfd == epoll::hostFd)
        {
            //新链接
            acceptNewConnect(epoll::hostFd);
        }
        else if (events_[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
        {
            //出现异常，断开连接
            if(client::clients[newfd])
                timerManager::delTimer(client::clients[newfd]->clientTimer);
        }
        else if ( events_[i].events & EPOLLIN )
        {
            client::clients[newfd]->setRead();
            client::clients[newfd]->clientTimer.lock()->timeRefresh();
            std::shared_ptr<client>temc = client::clients[newfd];
            threadPool::taskAppend(temc);
        }
        else if (events_[i].events & EPOLLOUT)
        {//将writebuff发出即可
            if(client::clients[newfd].get()==NULL)
            {
                break;
            }
            client::clients[newfd]->setWrite();
            client::clients[newfd]->clientTimer.lock()->timeRefresh();
            std::shared_ptr<client>temc = client::clients[newfd];
            threadPool::taskAppend(temc);
        }
        else {}
    }
    //timerManager::clearDeads();
    return 0;
}


