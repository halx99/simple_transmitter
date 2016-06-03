#include "threadpool.h"

using namespace simplepp_1_0_2012_9::asy;

// basic_service
basic_service::basic_service(void) 
{
}

basic_service::~basic_service(void)
{
}

void basic_service::release(void)
{
    delete this;
}


// threadpool
threadpool::threadpool(void) : destroyed(false)
{
}

threadpool::~threadpool(void)
{
}

void threadpool::enqueue_head(basic_service* service)
{
    scope_lock<thread_mutex> lock(this->sq_locker);
    this->sq.push_front(service);
    this->notifier.signal();
}

void threadpool::enqueue_tail(basic_service* service)
{
    scope_lock<thread_mutex> lock(this->sq_locker);
    this->sq.push_back(service);
    this->notifier.signal();
}

void threadpool::enqueue_tail_n(basic_service** service_array, int n)
{
    scope_lock<thread_mutex> lock(this->sq_locker);
    for(int i = 0; i < n; ++i)
    {
        this->sq.push_back(*service_array);
        ++service_array;
        this->notifier.signal();
    }
}

void threadpool::enqueue_head_n(basic_service** service_array, int n)
{
    scope_lock<thread_mutex> lock(this->sq_locker);
    for(int i = 0; i < n; ++i)
    {
        this->sq.push_front(*service_array);
        ++service_array;
        this->notifier.signal();
    }
}

void threadpool::destroy(void)
{
    // set the thread destroyed value.
    this->destroyed = true;

    // notify all thread to stop working.
    this->notifier.broadcast();

    // wait all worker threads exit.
    this->wait();

    // close all worker threads kernel object.
    this->dispose();

    // destory the resource of threadpool
    delete this;
}

long threadpool::svc(void)
{
    basic_service* target = nullptr;

loop_finite:
        this->sq_locker.lock();
        while(!this->destroyed && this->sq.empty()) 
        {
            this->notifier.wait(this->sq_locker);
        }
        if(this->destroyed) {
            this->sq_locker.unlock(); // release the lock
            goto thread_exit_point;
        }
        
        // dequeue from head of service queue and release lock immediately
        target = (*this->sq.begin());
        this->sq.pop_front();
        this->sq_locker.unlock();

        // execute the head of the service queue, consider the infinite-loop
        target->entry();

        // go on to execute next service
        goto loop_finite;

thread_exit_point:
#ifdef _DEBUG
        printf("the thread: %d will exit.\n", thr_self());
#endif
        return thr_self();
}

