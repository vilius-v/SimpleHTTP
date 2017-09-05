#ifndef __HTTPRESP__
#define __HTTPRESP__

#include <cstring>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <locale>

class HttpResponse
{
protected:
	// Data members
	unsigned int contentLength;
	int statusCode;
	char* status;
	char* payload;
	char* response;
	char* protocolVersion;
	bool headerComplete; // true if header is complete, needed for the receiveMessage function
	
	// Protected  helper functions
	// Generate status message
	void genStatus(int statusC);
	void genProtocol(float protocol);
	// Parsing Helper
	void helper(char* buffer, char* &dataToStore);
	// Clear all pointers and buffers
	void clear();

public:
	// Default Constructor
	HttpResponse();
	// Parameterized Constructor 
	HttpResponse(double payloadLen, int StatCode, char* htmlPayload);
	HttpResponse(int errorStatus);
	// Destructor
	~HttpResponse();

	// Generate the HTTP request
	char* genReq();
	// Parse request
	void parseReq(char *buffer);
	
	// Get methods
	unsigned int getContentLength();
	int getStatusCode();
	char* getStatus();
	char* getPayload();
	char* getProtocolVersion();
	bool isHeaderComplete();
};

#endif