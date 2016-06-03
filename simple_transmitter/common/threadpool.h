#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include <list>
#include "thread_basic.h"
#include "thread_synch.h"

namespace simplepp_1_0_2012_9 {
namespace asy {

class basic_service;
class threadpool;

class basic_service
{
protected:
    virtual ~basic_service(void);
public:
    basic_service(void);
    void release(void);
public:
    virtual void entry(void) = 0;
};

class threadpool : public thread_basic
{
protected:
    ~threadpool(void);
public:
    threadpool(void);

    void destroy(void);

    void enqueue_tail(basic_service* service);
    void enqueue_head(basic_service* service);
    
    void enqueue_tail_n(basic_service** service_array, int n);

    void enqueue_head_n(basic_service** service_array, int n);

private:
    virtual long svc(void);

private:
    std::list<basic_service*> sq;                // service queue
    thread_mutex              sq_locker;         // service queue locker 
    thread_cond               notifier;          // wait until service queue not empty
    bool                      destroyed;         // is threadpool destroyed
};

}; // namespace: simplepp_1_0_2012_9::asy
}; // namespace: simplepp_1_0_2012_9

#endif
/*
* Copyright (c) 2012-2019 by X.D. Guo ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
***/

