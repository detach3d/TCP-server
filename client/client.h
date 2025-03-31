
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>

#include <arpa/inet.h>


void *get_in_addr(sockaddr *sa);

void client(int argc, char *argv[]);