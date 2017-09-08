# SimpleHTTP

Implementation of a multithreaded web server, where the client sends HTTP
GET requests to an arbitrary server, and receives an ASCII only HTTP response.

The multithreaded web server allows connections from multiple web clients, and
responds with an HTTP response, with the file, if it exists in the working
directory.


###INSTALLATION:

Assuming that the user is using Ubuntu 14.04, the following commands install
the client and server files:
1. For only the client, run make client
2. For only the server, run make server
3. For both implementations, run make
4. To uninstall, run make clean


###USAGE:

`./web−client <URL1> <URL2> <URL3> ...`

`./web-server <host name> <port number> <host_directory>`

The first command is the web-client implementation. The client accepts multiple
URLs as input, with the hostname, followed by a colon preceding the port
number, and ended with the file path that the user wants to get. The client will
assume that if no port is provided, the default port number is 4000. If no file is
specified, the client will send a request for ”/”.

For the web-server, the syntax is straightforward. If any argument is missed
in the web-server implementation, the default value for host name is set to
”localhost”, the port number to ”4000”, and the hosting directory to the current
directory. The host returns an error response in the case that the a file is not
accessible by the server (or does not exist).
