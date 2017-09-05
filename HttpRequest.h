#ifndef __HTTPREQ__
#define __HTTPREQ__

#include <iostream>
#include <cstring>
#include <string>
#include <locale>

class HttpGetRequest 
{
protected:
	// Protected datamembers
	char* path;
	char* host;
	char* protocolVersion;
	char* getReq;
	
	// Helper fuction
	void helper(char* buffer, char* &dataToStore);
	// Clear cstrings
	void clear();

public:
	// Default Constructor
	HttpGetRequest();
	// Parameterized Constructor
	HttpGetRequest(char* fpath, char* fhost);
	// Destructor
	~HttpGetRequest();
	// Generate the HTTP request
	char* genReq();
	// Parse request
	void parseReq(char *buffer);

  	// Return protected variables
	char* getPath();
  	char* getHost();
  	char* getProtocolVersion();
  	// int getErrorStatus();
};

#endif
