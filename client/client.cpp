/*
** client.c -- a stream socket client demo
*/

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void client(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	addrinfo hints = {}; 
    std::shared_ptr<addrinfo> servinfo(nullptr);
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 3) {
	    std::cout << "usage: client hostname\n";
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, reinterpret_cast<addrinfo **>(&servinfo))) != 0) {
		throw std::invalid_argument{"Getaddrinfo error"};
		//return 1;
	}
    addrinfo* p = servinfo.get();
	// loop through all the results and connect to the first we can
	for(; p != nullptr; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
                    throw std::invalid_argument{"Socket error"};
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			throw std::invalid_argument{"Connect error"};
			close(sockfd);
			continue;
		}

		break;
	}

	//freeaddrinfo(servinfo.get()); // all done with this structure

	if (p == nullptr) {
		throw std::invalid_argument{"Failed to connect"};
		//return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof(s));
	std::cout << "client: connecting to " << s << '\n';

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    throw std::invalid_argument{"Failed to receive"};
	    exit(1);
	}

	buf[numbytes] = '\0';

	std::cout << "client: received "<< buf << '\n';

	close(sockfd);

	//return 0;
}

