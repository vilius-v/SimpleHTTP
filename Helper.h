#ifndef __HELPERCLASS__
#define __HELPERCLASS__

#include <cstring>
#include <string>
#include <cstdlib>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


// The length of each segment we will accept
const int BUFFER_LEN = 512;


// Function declarations
int sendMessage(int clientSockfd, const char* msg, int len);
int receiveMessage(int sockfd, char* &result, int end);
int errorStatus(char* &path, char* protocolVersion);
void getFilename(char* &fPath);

#endif