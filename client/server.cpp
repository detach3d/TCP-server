/*
** server.c -- a stream socket server demo
*/

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <exception>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "server.h"
#include <memory>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
	static_cast<void>(s); // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, nullptr, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((reinterpret_cast<sockaddr_in*>(sa))->sin_addr));
	}

	return &((reinterpret_cast<sockaddr_in6*>(sa))->sin6_addr);
}

void server()
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	addrinfo hints = {};  // Initialize hints to zero
    std::unique_ptr<addrinfo> servinfo(nullptr);
    
	sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(nullptr, PORT, &hints, reinterpret_cast<addrinfo **>(&servinfo))) != 0) {
		throw std::invalid_argument{"Getaddrinfo error"};
		//return 1;
	}
    auto* p = servinfo.get();
	// loop through all the results and bind to the first we can
	for(; p != nullptr; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
                    throw std::invalid_argument{"Socket error"};
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
                    throw std::invalid_argument{"Setsockopt error"};
			//exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			throw std::invalid_argument{"Bind error"};
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo.get()); // all done with this structure

	if (p == nullptr)  {
		throw std::invalid_argument{"Failed to bind"};
		//exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		throw std::invalid_argument{"Listen error"};
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
		throw std::invalid_argument{"sigaction error"};
		//exit(1);
	}

	std::cout << "server: waiting for connections...\n";

	while(1) {  // main accept() loop
		sin_size = sizeof(their_addr);
		new_fd = accept(sockfd, reinterpret_cast<sockaddr*>(&their_addr), &sin_size);
		if (new_fd == -1) {
			throw std::invalid_argument{"Accept error"};
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr(reinterpret_cast<sockaddr*>(&their_addr)),
			s, sizeof s);
		std::cout << "server: got connection from " << s << '\n';

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!", 13, 0) == -1)
            throw std::invalid_argument{"Send error"};
			close(new_fd);
			//exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	
}

