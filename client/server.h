
#include <arpa/inet.h>


void sigchld_handler(int s);

void *get_in_addr(sockaddr *sa);

void server();