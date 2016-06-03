#include "transmission_basic.h"


transmission_basic::transmission_basic()
{
	this->sock_obj = new xxsocket(AF_INET, SOCK_STREAM, 0);
}

transmission_basic::~transmission_basic()
{
    delete this->sock_obj;
}
