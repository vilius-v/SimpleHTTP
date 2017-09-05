# CS118 - Lab 1

Our implementation of a multithreaded web server, where the client sends HTTP
GET requests to an arbitrary server, and receives an ASCII only HTTP response.

The multithreaded web server allows connections from multiple web clients, and
responds with an HTTP response, with the file, if it exists in the working
directory.

INSTALL: 
Client-Server: run "make" from the same directory that you found the readme
Client: run "make client"
Server: run "make server"
Parser Test: to test the parser, run "make parsertest"

UNINSTALL:
run "make clean"
