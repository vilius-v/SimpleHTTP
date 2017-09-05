#include "HttpRequest.h"

// Default constructor
HttpGetRequest::HttpGetRequest()
{
	// Initalize all pointers to NULL
	this->path = NULL;
	this->host = NULL;
	this->getReq = NULL;
	this->protocolVersion = NULL;

	// Error code to 0
	//this->error = 0;
}

// Destructor
HttpGetRequest::~HttpGetRequest()
{
	this->clear();
}

// Parameterized Constructor
HttpGetRequest::HttpGetRequest(char* fpath, char* fhost)
{
	// Set values
	this->path = new char [std::strlen(fpath)];
	std::strcpy(this->path, fpath);
	this->host = new char [std::strlen(fhost)];
	std::strcpy(this->host, fhost);
	this->protocolVersion = new char[9];
	std::strcpy(this->protocolVersion, "HTTP/1.0");

}

void HttpGetRequest::helper(char* buffer, char* &dataToStore)
{
	std::string temp = "";
	for(int i=0; ((buffer[i] != '\r') && (buffer[i] != ' ')); i++)
	{
		temp += buffer[i];
	}

	dataToStore = new char [temp.length()];
	std::strcpy(dataToStore, temp.c_str());
}

char* HttpGetRequest::genReq()
{
	std::string get = "";
	get += "GET ";
	get += this->path;
	get += " ";
	get += this->protocolVersion;
	get += "\r\nHost: ";
	get += this->host;
	get += "\r\n\r\n";
	this->getReq = new char [get.length()+1];
	std::strcpy(this->getReq, get.c_str());
	return this->getReq;
}

//  Parse the buffer
void HttpGetRequest::parseReq(char* buffer) 
{
	// Start by clearing the buffer
	this->clear();

	// C++ strings are so ez
	std::string getR = buffer;

	//std::cout << std::endl << "buffer is " << buffer << std::endl ;

	for(unsigned int i = 0; i < getR.length(); i++)
	{
		// Parse GET
		if(getR[i] == 'G')
		{
			// Try and store "GET " in the c++ string
			std::string check = "";
			check += getR[i];
			check += getR[i+1];
			check += getR[i+2];
			check += getR[i+3];

			// Check if the string that was stored is "GET "
			if(check == "GET ")
			{
				int pathSize;	// Store the length of the path string

				// Accept the path size
				this->helper(buffer+(i+4), this->path);
				// std::cout << std::endl << "Path: " << this->path << std::endl;
				
				// Compute the path size, and send in this everything after the "GET <path> "
				pathSize = std::strlen(this->path);

				// Parse the protocol version
				this->helper(buffer+(i+5+pathSize), this->protocolVersion);
			}
		}

		// Parse Host
		if(getR[i] == 'H' || getR[i] == 'h')
		{
			// Try and store "Host  " in the c++ string
			std::string check = "";
			check += getR[i];
			check += getR[i+1];
			check += getR[i+2];
			check += getR[i+3];
			check += getR[i+4];
			check += getR[i+5];

			// Make string lowercase
			for(size_t ctr = 0; ctr < check.length(); ctr++)
				check[ctr] = tolower(check[ctr]);

			// Check if the string that was stored is "Host "
			if(check == "host: ")
				this->helper(buffer+(i+6), this->host);
		}
	}

}


// Clear members
void HttpGetRequest::clear()
{
	// Delete all mallocd pointers, set error to 0,and protocolVersion to HTTP/1.0
	delete this->path;
	delete this->host;
	delete this->protocolVersion;
	delete this->getReq;
	// this->error = 0;
	this->path = NULL;
	this->host = NULL;
	this->getReq = NULL;
	this->protocolVersion = NULL;
}

// Access methods
char* HttpGetRequest::getPath() 
{
	return this->path;
}

char* HttpGetRequest::getHost() 
{
	return this->host;
}

char* HttpGetRequest::getProtocolVersion() 
{
	// std::cout << std::endl << this->protocolVersion << std::endl;	
	return this->protocolVersion;
}
