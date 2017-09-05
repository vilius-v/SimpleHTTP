/*
    A lot of the initial socket code was influenced by the examples placed by Beej
    in the Network Programming Guide; we found his section on GetAddrInfo especially
    helpful when developing an initial client-server model.
*/


// C++ headers
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <thread>
#include <csignal>

// Kernel Includes
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

// User-Defined module includes
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Helper.h"

using namespace std;

typedef struct addrinfo AddressInfo;
typedef struct sockaddr_in SocketAddress;

//const int BUFFER_LEN = 20;

// these have to be global for signal handler to use them
int socketDsc;                              // Socket descriptor
bool endAll = false; // ends all infinite loops

// Server parameters
char* hostName;     // IP or Domain that we need to connect to 
char* hostDir;      // Path of directory to host on the server
char* port;         // Port number to open socket

// Handles parsing and errors 
void parse( int argcount, char *argval[])
{
    // Only 1 argument; set all values to default
    if ( argcount == 1 )
    {
        // Set hostname
        hostName = new char [10];
        strcpy(hostName, "localhost");

        // Set port
        port = new char [5];
        strcpy(port, "4000");

        // Set directory
        hostDir = new char [2];
        strcpy(hostDir, ".");

    }
    // Hostname set, rest default
    else if ( argcount == 2 )
    {
        // argv[1] is the hostname
        hostName = argval[1];

        // Set port
        port = new char [5];
        strcpy(port, "4000");

        // Set directory
        hostDir = new char [2];
        strcpy(hostDir, ".");
    }
    // Hostname and port set, rest default
    else if ( argcount == 3 )
    {
        // argv[1] is the hostname
        hostName = argval[1];

        // argv[2] is the port number
        port = argval[2];
        int portNum = atoi(argval[2]);
        if(portNum <= 0 || portNum > 65535)
        {
            cerr << "Error: port number is invalid" << endl;
            exit(1);
        }

        // Set directory
        hostDir = new char [2];
        strcpy(hostDir, ".");

    } 
    // Parse values for all values set
    else 
    {
        // argv[1] is the hostname
        hostName = argval[1];

        // argv[2] is the port number
        port = argval[2];
        int portNum = atoi(argval[2]);
        if(portNum <= 0 || portNum > 65535)
        {
            cerr << "Error: port number is invalid" << endl;
            exit(1);
        }

        // argv[3] is the hosting directory
        hostDir = argval[3];
    }
}

char* getFullPath (char* relPath) { 
    string path = hostDir; 
    path += relPath; 
    path += '\0'; 
    char* fullpath = new char[path.length()]; 
    strcpy(fullpath, path.c_str()); 
    return fullpath; 
} 

// ToDo:
// Multithreaded handler for each client
void clientHandler (int fileDsc) 
{
    char* req;          // Request buffer

    // Obtain the Request, and parse it using the user defined class
    receiveMessage(fileDsc, req, endAll);
    HttpGetRequest o;
    o.parseReq(req);

    /*
    // Debugging only: Print out the information of the obtained Request
    cout << "Length: " << reqlen << endl;
    cout << "Path: " << o.getPath() << endl;
    cout << endl << "Host: " << o.getHost() << endl;
    cout << endl << "Protocol Version: " << o.getProtocolVersion() << endl;
    cout << endl << "Generated HttpRequest: " << endl << o.genReq();
    */
    
    char* fullpath = getFullPath(o.getPath());

    int status = errorStatus(fullpath, o.getProtocolVersion());

    HttpResponse *resp;
    string fileDataStr = "";
    int fileSize = 0;
    int bytesRead = 0;
    int totalBytesRead = 0;
    int fd;

    if (status != 200) {
        // error, send error response
        resp = new HttpResponse(status);
    } else {
        cerr << "Opening... " << endl; 
        // Create the file 
        fd = open(fullpath, O_RDONLY, 0666);
        struct stat stat_buf;
        int rc = fstat(fd, &stat_buf);
        if (fd <= 0 || rc != 0) {
            // error opening file, send 404 response
            cerr << "Error opening file " << fullpath << endl;
            resp = new HttpResponse(404);
        } else {
            fileSize = stat_buf.st_size;
            char* nullbyte = new char[1];
            nullbyte[0] = '\0';
            resp = new HttpResponse(fileSize, 200, nullbyte);
        }
    }

    string respStr = resp->genReq();
    sendMessage(fileDsc, respStr.c_str(), respStr.length());

    if (fileSize > 0) {

        char* fileData = new char[BUFFER_LEN+1];
        fileData[BUFFER_LEN] = '\0'; // add null terminater
        while ((bytesRead = read(fd, fileData, BUFFER_LEN)) > 0) {
            sendMessage(fileDsc, fileData, bytesRead);
            totalBytesRead += bytesRead;
        }

        if (totalBytesRead < fileSize) {
            cerr << "Actual file size does not match fstat file size!" << endl;
            // keep sending null bytes until the amount we send matches the value we sent in ContentLength
            int bytesLeft = fileSize-totalBytesRead;
            fileData = new char[bytesLeft];
            memset(fileData, 0, bytesLeft);
            sendMessage(fileDsc, fileData, bytesLeft);
        }

        cerr << "Sent " << totalBytesRead << "/" << fileSize << " bytes" << endl;
    }


    cerr << "done" << endl;

    // Close the file descriptor for this instance
    close(fileDsc);
}

// Initialize socket, and return the file descriptor 
void initializeSocket(char* &hostN, char* &portN, int *socketDesc)
{   
    // Socket addresses; I don't know if I can move these into initialize()
    AddressInfo hints, *servInfo, *p;    // getaddrinfo structs

    // Socket Procedures:
    // Zero out the hints, and set its values
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          // Assuming IPv4
    hints.ai_socktype = SOCK_STREAM;    //TCP socket

    if ((getaddrinfo(hostN, portN, &hints, &servInfo) != 0)) 
    {
        cerr << "Call to getaddrinfo failed; check hostname and/or port number" << endl;
        exit(1);
    }

    // Loop through getaddrinfo results, until connection has been established
    for(p = servInfo; p != NULL; p = p->ai_next) 
    {
        if ((*socketDesc = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            // cerr << "Socket could not establish file descriptor" << endl;
            continue;
        }

        if (bind(*socketDesc, p->ai_addr, p->ai_addrlen) == -1) 
        {
            // cerr << "Could not connect to socket" << endl;
            close(*socketDesc);
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    // What if we couldn't connect with any host in the servinfo list?
    if (p == NULL) 
    {
        cerr << "Failed to connect to server" << endl ;
        close(*socketDesc);
        exit(1);
    } 

    freeaddrinfo(servInfo); // all done with this structure
}

void listenLoop (int socketDesc) 
{
    int incFileDesc;                     // Descriptor for each established connection
    socklen_t temp;                      // Temp for sizeof(clientAdd)
    SocketAddress clientAdd;             // One day, I will figure out why we need this

    // Listen for incoming connections. Queue up to 5 requests     
    listen(socketDesc, 5);

    // Multithread after this:
    // Continuously accept connections, and create a new clientHandler thread for each connection
    while (!endAll) {

        // Set the file descriptor to 
        temp = sizeof(clientAdd);
        incFileDesc = accept(socketDesc, (struct sockaddr *) &clientAdd, &temp);
        if (incFileDesc < 0) 
        {
            cerr << "Could not accept connection" << endl;
            close(socketDesc);
            exit(0);
        }

        thread handler = thread(clientHandler, incFileDesc);
        handler.detach(); // run in background, destroy automatically on end

        // Debugging only: Print if successful
        cerr << "Successfully obtained descriptor" << endl;
    }
}

// Interrupt signal handler
void interruptHandler (int signum) 
{
    cerr << "Interrupted! All descriptors closed" << endl;
    endAll = true;      // End all infinite loops
    // Don't we need to join the threads here?
    close(socketDsc);   // Close socket to interrupt the socket accept() function
    exit(0);
}

int main ( int argc, char *argv[] )
{
    // Parse the input
    parse(argc, argv);

    // Initialize the scoket desctiptor
    initializeSocket(hostName, port, &socketDsc);

    // Register signal SIGINT and signal handler  
    signal(SIGINT, interruptHandler); 

    // This handles connections to the server
    listenLoop(socketDsc);

    // Loop exited on its own, must have run into error
    close(socketDsc);

    /*
    //Debugging only: Test if the descriptor can write
    char buffer[32];
    strcpy(buffer, "Hello, Socket World!");
    int n = write(FileDsc,buffer,strlen(buffer));
    if (n < 0) 
        cout << "Error writing to socket" << endl;
    */

    /*
    //Debugging only: Check if the parsing was done correctly
    cout << "Host: " << hostName << endl;
    cout << "Port: " << port << endl;
    cout << "Hosting Directory: " << hostDir << endl;
    */

    // Avoid errors
    return 0;
}