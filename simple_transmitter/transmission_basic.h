// transmission_basic.h
#ifndef _TRANSMISSION_BASIC_H_
#define _TRANSMISSION_BASIC_H_
#include "transmission_def.h"

class transmission_basic
{
public:
	transmission_basic();
	virtual ~transmission_basic();
	virtual bool start(const char* ip, u_short port) = 0;
	virtual bool is_server(void) { return false; }
protected:
	xsocket* sock_obj;
};

#endif

/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/
