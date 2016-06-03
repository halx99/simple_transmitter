// transmission_server.h
#ifndef _TRANSMISSION_SERVER_H_
#define _TRANSMISSION_SERVER_H_
#include <fstream>
#include "transmission_basic.h"
#include <mutex>
#include <thread>
#include <vector>
class transmission_server : public transmission_basic
{
public:
	transmission_server();

	bool start(const char* ip, u_short port);

	virtual bool is_server(void) { return true; }

	void run(void);
private:
	std::vector<std::thread*> threads;
};

#endif

/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/

