# Makefile
GPP=g++
CFLAGS= -g -Wall -Werror
MTOPTIONS =-pthread -std=c++11 

# Targets
all: client server
client: client.o HttpRequest.o HttpResponse.o Helper.o
	$(GPP) client.o HttpRequest.o HttpResponse.o Helper.o -o web-client
server: server.o HttpRequest.o HttpResponse.o Helper.o
	$(GPP) server.o HttpRequest.o HttpResponse.o Helper.o -o web-server $(MTOPTIONS)
client.o:
	$(GPP) $(CFLAGS) -c client.cpp -o client.o   
server.o:
	$(GPP) $(CFLAGS) -c server.cpp -o server.o $(MTOPTIONS)
HttpRequest.o:
	$(GPP) $(CFLAGS) -c HttpRequest.cpp -o HttpRequest.o
HttpRespose.o:
	$(GPP) $(CFLAGS) -c HttpResponse.cpp -o HttpResponse.o
Helper.o:
	$(GPP) $(CFLAGS) -c Helper.cpp -o Helper.o	
clean:
	rm -f *.o *.out web-server web-client
