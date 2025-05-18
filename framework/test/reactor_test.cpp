// TODO! for Shimons tests: use ( telnet 127.0.0.1 9035 ) in a different shell to paly TCP ping pong. run "telnet 127.0.0.1 9035" and send "ping" to the server -> expect to receive "pong". then, send "ping" in the same shell as the reactor -> expect to receive "pong". then, send "q" in the same shell as the reactor -> expect the reactor to stop and end the process.      updated at 09:13 25/dec/2024. 



#define _POSIX_C_SOURCE 200112L

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>
#include "reactor.hpp"

#define PORT "9035"
using namespace ilrd;
fd_set read_fds;  /* temp file descriptor list for select() read */
fd_set write_fds; /* temp file descriptor list for select() write */
int fdmax;        /* maximum file descriptor number */

int listener_tsp, listener_udp;     /* listening socket descriptor */
int newfd;        /*  newly accept()ed socket descriptor */
struct sockaddr_storage remoteaddr; /*  client address */
socklen_t addrlen;

void AddToSet(fd_set* set, int fd_to_add);
void TCPWaitForConnection();
void TryToRead(int socket, Reactor::Mode mode);

std::shared_ptr<Reactor> reac;
class ReactorListener : public Reactor::Ilistener
{
public:
	std::vector<Reactor::ListenPair> Listen(
							const std::vector<Reactor::ListenPair>& listenTo)
	{
		AddToReaders(listenTo);
		TCPWaitForConnection();
		return SetAwakenReadSockets();
	}
private:
	void AddToReaders(const std::vector<Reactor::ListenPair>& listenTo)
	{
		FD_ZERO(&read_fds);
		for (auto pair : listenTo)
			(pair.second == Reactor::READ)
			?
			AddToSet(&read_fds, pair.first)
			:
			AddToSet(&write_fds, pair.first);
	}

	std::vector<Reactor::ListenPair> SetAwakenReadSockets()
	{
		std::vector<Reactor::ListenPair> ret;
		for (int i = 0; i < fdmax+1; ++i)
			if (FD_ISSET(i, &read_fds))
				ret.push_back(Reactor::ListenPair(i, Reactor::READ));
			else if (FD_ISSET(i, &write_fds))
				ret.push_back(Reactor::ListenPair(i, Reactor::WRITE));
		return ret;
	}
};

char buf[256];   /*  buffer for client data */
int nbytes;

char remoteIP[INET6_ADDRSTRLEN];

int yes=1;        /*  for setsockopt() SO_REUSEADDR, below */
int i, j, rv, run = 1;

struct addrinfo hints, *ai, *p;

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


static int TCPCreateSocket(int *socket_fd, int type)
{
    FD_ZERO(&read_fds);
	FD_ZERO(&read_fds);
	

	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        return 1;
    }
	p = ai;
	*socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	if (*socket_fd < 0)
	{
		fprintf(stderr, "failed to creat socket: %s\n", gai_strerror(rv));
		return 1;
	}
	fcntl(*socket_fd, F_SETFL, O_NONBLOCK);
	return 0;
}

static int TCPBind(int *socket_fd, int flag)
{
	int yes = 1;
	if (setsockopt(*socket_fd, SOL_SOCKET, flag, &yes, sizeof(yes)) == -1)
	{
		perror("setsockopt");
		return 1;
	} 
	if (bind(*socket_fd, p->ai_addr, p->ai_addrlen) < 0)
	{
		fprintf(stdout, "%s\n", strerror(errno));
		close(*socket_fd);
		fprintf(stderr, "failed to bind: %s\n", strerror(errno));
		freeaddrinfo(ai);
		return 1;
	}
	

	freeaddrinfo(ai); /* all done with this */
	return 0;
} 

void AddToSet(fd_set* set, int fd_to_add)
{
	FD_SET(fd_to_add, set);
	if (fd_to_add > fdmax)
	{
		fdmax = fd_to_add;
	}
}

static int TCPListen()
{
	if (listen(listener_tsp, 100) == -1)
	{
        perror("listen failed\n");
		close(listener_tsp);
        return 1;
    }
	fdmax = listener_tsp;
	return 0;
}


void TCPSetNewConnection(int fd, Reactor::Mode mode)
{
	addrlen = sizeof remoteaddr;
	newfd = accept(listener_tsp, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
	{
		perror("accept failed\n");
		return;
	}
	reac->Register(newfd, Reactor::READ, TryToRead);
    printf("new connection from %s on socket %d\n",
	inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr),
											remoteIP, INET6_ADDRSTRLEN), newfd);
	fflush(stdout);
	return;
}

static void Quit()
{
	reac->Stop();
	for (int k = 1; k < fdmax; ++k){close(k);}
}

static int DidClientHangUp(int socket){return (0 > nbytes) ?  1 : 0;}

int SendPong(int socket)
{
	if(write(socket, "pong\n", nbytes) == -1)
		perror("send failed\n"); 
		return 1;
	return 0;
}

void ParsMsg(int socket)
{
	if (STDIN_FILENO == socket)
	{
		socket = STDOUT_FILENO;
		buf[nbytes] = '\0';
		if (0 == strcmp("q\n", buf))
		{
			Quit();
			return;
		}
	}
	else
	{
		buf[nbytes - 1] = 0;
		buf[nbytes - 2] = '\n';
	}
	
/* 	printf("socket: %d sends: %s", socket, buf);
 */	fflush(stdout);
	if (0 == strcmp("ping\n", buf))
	{
		if(SendPong(socket))
		{
			return;
		}
	}
	return;
}

void TryToRead(int socket, Reactor::Mode mode)
{
	int res = 0;
	nbytes = 0;
	memset(buf, 0, sizeof(buf));
	nbytes = read(socket, buf, sizeof(buf));
	if (0 >= nbytes)
	{
		res = DidClientHangUp(socket);
		reac->UnRegister(socket, Reactor::READ);
		close(socket);
		return ;
	}
	ParsMsg(socket);
}

void UDPRead(int data_fd, Reactor::Mode mode)
{
	struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof(their_addr);

	nbytes = 0;
	memset(buf, 0, sizeof(buf));
    if ((nbytes = recvfrom(data_fd, buf, sizeof(buf) - 1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom\n");
        return;
    }
	buf[nbytes] = 0;
	
	if (-1 == sendto(data_fd, "pong\n", sizeof("ping\n"), 0,
									(struct sockaddr *)&their_addr, addr_len))
    {
        perror("Server: sendto\n");
        return;
    }
	printf("data_fd: %d, buf: %s\n", i, buf);
			fflush(stdout);
}

void TCPWaitForConnection()
{
	if (-1 == select(fdmax+1, &read_fds, &write_fds, NULL, NULL))
		perror("select failed\n");
}

static int TCPInit()
{
	
	return TCPCreateSocket(&listener_tsp, SOCK_STREAM)
			||
			TCPBind(&listener_tsp, SO_REUSEADDR)
			||
			TCPCreateSocket(&listener_udp ,SOCK_DGRAM)
			||
			TCPBind(&listener_udp, SO_BROADCAST);
}

int ShimonsTests()
{
	if (TCPInit()){return 1;}
	TCPListen();
	std::shared_ptr<Reactor::Ilistener> ptr(new ReactorListener);
	reac = std::make_shared<Reactor>(ptr);
	reac->Register(listener_tsp, Reactor::READ, &TCPSetNewConnection);
	reac->Register(listener_udp, Reactor::READ, &UDPRead);
	reac->Register(0, Reactor::READ, &TryToRead);
	reac->Run();
	return 0;
}

int main()
{
	std::cout << "Pleas start by writing a simple test first!!!!" << std::endl;

	ShimonsTests();
	return 0;
}
