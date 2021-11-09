#pragma once
#include<unistd.h>
#include<fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <memory>
#include <netinet/in.h>
using namespace::std;
int setNonBlocking(int fd);
int setHostSock(int port);
string numtostr(int num);
