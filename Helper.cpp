#include "Helper.h"

const int SLEEP_LENGTH = 10;
const int MAX_SLEEP_TIME = 1000;

// Send a message to the server/client
int sendMessage(int clientSockfd, const char* msg, int len) 
{
    // Send cstring; return error if nothing was sent
    if (send(clientSockfd, msg, len, MSG_NOSIGNAL) == -1) 
    {
        std::cerr << "Could not send requset" << std::endl;
        exit(1);
    }
    return 0;
}

// General purpose message receiver
// Assumes message is finished only if connection closes or read times out
// Note that if sender finishes sending without closing the connection (eg. listening for response),
// a blocking recv() call will just hang
// This is why we use the MSG_DONTWAIT flag to set recv to non-blocking
int receiveMessage(int sockfd, char* &result, int end) 
{
    // send/receive data to/from connection
    char buf[BUFFER_LEN] = {0};
    std::string raw = "";
    int bytesRead = 0;                  // Used to count the amount of bytes sent over the descriptor
    int currSleepLength = 0;

    while (!end) 
    {
        memset(buf, '\0', sizeof(buf));     // Zero out the buffer

        // Receive over the file descriptor
        bytesRead = recv(sockfd, buf, BUFFER_LEN-1, MSG_DONTWAIT);

        if (bytesRead > 0) {

            currSleepLength = 0; // whenever anything is read, reset timeout
            raw += buf;

        } else if (bytesRead == 0) { // connection close
            break;

        } else { // bytesRead < 0 means no data read, but connection still open
            if (currSleepLength > MAX_SLEEP_TIME) {
                break;
            }

            usleep(SLEEP_LENGTH);
            currSleepLength += SLEEP_LENGTH;
            continue;
        }
    }

    // either connection close or timeout
    result = new char[raw.length()+1];
    result[raw.length()] = '\0'; // add null terminator
    std::strncpy(result, raw.c_str(), raw.length());

    return bytesRead; // return whatever value caused the whileloop to break (0 for connection close, -1 for timeout)
}

// Checks if there is an error in the passed in string
int errorStatus(char* &path, char* protocolVersion)
{
    int flag = 0;

    // Its simple to treat / as 404
    if(std::strcmp(path, "/") == 0)
    {
        return 404;
    }
    // Else check if the file exists, like one usually would
    else
    {
        if(access(path, R_OK) == 0)
            flag = 200;
        else
            return 404;
    }

    // // Check if the protocol version is valid
    // if(std::strcmp(protocolVersion, "HTTP/1.0") == 0)
    // {
    //     flag = 200;
    // }
    // else
    // {
    //     return 400;
    // }

    // Return the result
    return flag;
}

void getFilename(char* &filePath)
{
    std::string fPath(filePath);
    std::string temp = "";

    // Handle index.html
    if(fPath == "/")
    {
        fPath = "index.html";
        delete filePath;
        filePath = new char [fPath.length()];
        std::strcpy(filePath, fPath.c_str());
        return;
    }

    for(size_t i = (fPath.length() - 1); i > 0; i--)
    {
        if(fPath[i] != '/')
        {
            temp += fPath[i];
        }
        else
        {
            break;
        }
    }

    fPath = "";
    for(size_t i = 0; i < temp.length(); i++)
    {
        fPath[i] = temp[(temp.length()-1)-i];
    }
    fPath[temp.length()] = 0;

    delete filePath;
    filePath = new char [fPath.length()];
    std::strcpy(filePath, fPath.c_str());
}