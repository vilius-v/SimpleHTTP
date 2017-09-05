#include "HttpResponse.h"

HttpResponse::HttpResponse()
{
	this->contentLength = 0;
	this->statusCode = 0;
	this->payload = NULL;
	this->response = NULL;
	this->status = NULL;
	this->protocolVersion = NULL;
	this->headerComplete = false;
}

// Default Constructor
HttpResponse::HttpResponse(double payloadLen, int StatCode, char* htmlPayload)
{
	this->contentLength = payloadLen;
	this->statusCode = StatCode;
	this->genStatus(this->statusCode);
	this->payload = new char [std::strlen(htmlPayload)];
	std::strcpy(this->payload, htmlPayload);
	this->genProtocol(1.0);
	this->headerComplete = true;
}

// Error Response 
HttpResponse::HttpResponse(int errorStatus)
{
	this->contentLength = 0;
	this->statusCode = errorStatus;
	this->genStatus(this->statusCode);
	this->payload = NULL;
	this->response = NULL;
	this->genProtocol(1.0);
	this->headerComplete = false;
}

// Destructor
HttpResponse::~HttpResponse()
{
	this->clear();
}

// Parsing helper function
void HttpResponse::genProtocol(float protocol)
{
	std::string temp = "";
	if (protocol >= 1.1) {
		temp = "HTTP/1.1";
	} else {
		temp = "HTTP/1.0";
	}

	this->protocolVersion = new char [temp.length()];
	std::strcpy(this->protocolVersion, temp.c_str());
}

// Parsing helper function
void HttpResponse::helper(char* buffer, char* &dataToStore)
{
	std::string temp = "";
	for(int i=0; ((buffer[i] != '\r') && (buffer[i] != ' ')); i++)
	{
		temp += buffer[i];
	}

	dataToStore = new char [temp.length()];
	std::strcpy(dataToStore, temp.c_str());
}

// Clean up pointers and zero out data
void HttpResponse::clear()
{
	this->contentLength = 0;
	this->statusCode = 0;
	delete this->status;
	delete this->payload;
	delete this->response;
	delete this->protocolVersion;
	this->status = NULL;
	this->payload = NULL;
	this->response = NULL;
	this->protocolVersion = NULL;
	this->headerComplete = false;
}

// Get the response code string
void HttpResponse::genStatus(int statusC)
{
	if (statusC == 200)
	{
		this->status = new char [3];
		std::strcpy(this->status, "OK");	 
	}
	else if(statusC == 404)
	{
		this->status = new char[10];
		std::strcpy(this->status, "Not found");
	}
	else
	{
		this->status = new char [12];
		std::strcpy(this->status, "Bad request");
	}
}

// Generate the HTTP request
char* HttpResponse::genReq()
{
	// ints to strings
	std::stringstream A;
	A << this->contentLength;
	std::string contentLengthStr = A.str();

	// ints to strings again
	std::stringstream B;
	B << this->statusCode;
	std::string statusCodeStr = B.str();

	//std::string contentLengthStr = std::to_string((int)(this->contentLength));
	//std::string statusCodeStr = std::to_string((int)(this->statusCode));

	std::string temp = "";
	temp += this->protocolVersion; //"HTTP/1.0";
	temp += " ";
	temp += statusCodeStr;
	temp += " ";
	temp += this->status; 
	temp += "\r\n";
	temp += "Content-Length: "; 
	temp += contentLengthStr;
	temp += "\r\n\r\n";
	if (payload != NULL) temp += payload;
	this->response = new char [temp.length()+1];
	std::strcpy(this->response, temp.c_str());
	return this->response;
}

void HttpResponse::parseReq(char *buffer)
{
	// Start by clearing the buffer
	this->clear();

	// C++ strings are so ez
	std::string getR = buffer;

	//std::cout << std::endl << "buffer is " << buffer << std::endl ;

	for(unsigned int i = 0; i < getR.length(); i++)
	{
		// protocolVersion, statusCode, status are all on line 1
		if(i == 0)
		{
			// Bytes to skip, till we get to the start of the next value to be parsed
			int sLength = 0;

			// Parse protocolVersion
			this->helper(buffer, this->protocolVersion);

			// Parse statusCode
			char *stCd = NULL;
			sLength += std::strlen(this->protocolVersion) + 1; 	
			this->helper(buffer+sLength, stCd); // Store string 
			this->statusCode = atoi(stCd);	// Parse string as number
			sLength += std::strlen(stCd) + 1; // Update length to skip
			delete stCd;	// We thank you for your help

			// Parse status message
			this->genStatus(this->statusCode);
		}

		// Parse Content-Length
		if(getR[i] == 'C')
		{
			// Try and store "Content-Length: " in the c++ string
			std::string check = "";
			check += getR.substr(i, 16);

			// Check lowercase
			for(size_t ctr = 0; ctr < check.length(); ctr++)
				check[ctr] = tolower(check[ctr]);

			// Check if the string that was stored is "GET "
			if(check == "content-length: ")
			{
				char* contentSize;	// Store the length in ASCII
				this->helper(buffer+i+16, contentSize); // Store the value
				this->contentLength = (unsigned int) atoi(contentSize); // Parse as integer
			}
		}

		// Parse payload
		if(getR[i] == '\r')
		{
			std::string check = "";
			check += getR.substr(i, 4);

			// Check for double '\r\n\r\n'
			if (check == "\r\n\r\n")
			{
				this->headerComplete = true;
				int payloadLen = this->contentLength > 0 ? this->contentLength : strlen(buffer+i+4);

				// The rest is data
				this->payload = new char [payloadLen+1];
				strncpy(this->payload, buffer+i+4, payloadLen);
				this->payload[payloadLen] = 0;
			}
		}
	}
}

unsigned int HttpResponse::getContentLength()
{
	return this->contentLength;
}

int HttpResponse::getStatusCode()
{
	return this->statusCode;
}

char* HttpResponse::getStatus()
{
	return this->status;
}

char* HttpResponse::getPayload()
{
	return this->payload;
}

char* HttpResponse::getProtocolVersion()
{
	return this->protocolVersion;
}

bool HttpResponse::isHeaderComplete()
{
	return this->headerComplete;
}
