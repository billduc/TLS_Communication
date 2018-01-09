#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <sys/socket.h>
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

#define FAIL -1

//open listener, create server socket
int OpenListener(int port){
	int sd;
	struct sockaddr_in addr;

	sd = socket(PF_INET, SOCK_STREAM, 0);

	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if ( bind(sd, &addr, sizeof(addr) ) != 0){
		cerr << "can't bind socket";
		abort();
	}

	if (listen(sd, 123) != 0){
		cerr << "can't configure listening port";
		abort();
	}

	return sd;
}

//init server CTX - initialize SSL server and create context


