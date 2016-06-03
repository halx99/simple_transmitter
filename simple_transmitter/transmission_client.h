// transmission_client.h
#ifndef _TRANSMISSION_CLIENT_H_
#define _TRANSMISSION_CLIENT_H_
#include <fstream>
#include "transmission_basic.h"

class transmission_client : public transmission_basic
{
public:
	transmission_client();

	bool start(const char* ip, u_short port);
};

#endif

/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/
