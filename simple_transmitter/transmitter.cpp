#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "transmission_client.h"
#include "transmission_server.h"
#include "common/thread_basic.h"
//using namespace libsimple::net;
//using namespace libsimple::asy;
extern int server_thread_count;
int main(int argc, char** argv)
{
	transmission_basic* transmitter = nullptr;
	const char* role = nullptr;
	char* ip = nullptr;
	u_short port = 0;

	server_thread_count = 1;
	if(argc >= 4)
	{
		ip = argv[1];
		port = atoi(argv[2]);
		role = argv[3];

		if (strcmp("server", role) == 0) {
			transmitter = new transmission_server();

			std::cout << "your role is server.\n";

			if (argc >= 5) {
				int count = atoi(argv[4]);
				if (count != 0)
					server_thread_count = count;
			}
		}
		else if (strcmp("client", role) == 0) {
			transmitter = new transmission_client();
			std::cout << "your role is client.\n";
		}
		else {
			std::cerr << "you select a unknown role, usage: transmitter {ip} {port} {server|client}!\n";

			return -1;
		}
	}
	else {
		
		ip = "0.0.0.0";
		port = 3399;
		std::cout << "start default role: server, ";
		transmitter = new transmission_server();
	}
	
	std::cout << "listenning at 0.0.0.0:" << port << "...\n";
	
	transmitter->start(ip, port);

	return 0;
}

