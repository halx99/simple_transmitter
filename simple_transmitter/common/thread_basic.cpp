#include <signal.h>
#include "thread_synch.h"
#include "thread_basic.h"
#ifndef _WIN32
#include <string.h>
#endif

using namespace simplepp_1_0_2012_9::asy;

int thread_basic::spawn(THREAD_PROC base, void* arg, size_t stacksize, int flags, thread_t* pthr)
{
    thread_t thr;
    int retc = -1;
#ifdef _WIN32
    //#ifdef _WIN32_WCE
    thr = ::CreateThread(nullptr, stacksize, base, arg, flags, nullptr);
    //#else
    //    thr = (HANDLE)::_beginthreadex(nullptr, stacksize, (unsigned int(__stdcall*)(void*))base, arg, 0, nullptr);
	if(nullptr == thr) {
		retc = GetLastError();
	}
    //#endif
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, flags);
    pthread_attr_setstacksize(&attr, stacksize);
    retc = pthread_create(&thr, &attr, base, arg);
    pthread_attr_destroy(&attr);
#endif    
    if(pthr != NULL) {
        *pthr = thr;
    }
    return retc;
}

int thread_basic::join(thread_t& thr)
{
#ifdef _WIN32
    return WaitForSingleObject(thr, INFINITE);
#else
    return pthread_join(thr, NULL);
#endif
}

int thread_basic::join_n(thread_t* thrs, size_t number)
{
    int retc = 0;
#ifdef _WIN32
    retc = WaitForMultipleObjects(number, thrs, TRUE, INFINITE);
#else
    for(size_t i = 0; i < number; ++i) 
    {
        retc += thread_basic::join(thrs[i]);
    }
#endif
    return retc;
}

thread_basic::thread_basic(void) : _Myhead()
{
}

thread_basic::~thread_basic(void)
{
    //if(_Myhandle != 0)
    //{
    //    /**
    //    * int ret = pthread_kill(_Myhandle, 0);
    //    * switch(ret)
    //    * {
    //    * case ESRCH:  // The thread is not exists or already exit.
    //    *     break;
    //    * case EINVAL: // Sending singal illegal.
    //    *     break;
    //    * default:     // The thread running.
    //    *     _Terminate();
    //    * };
    //    */
    //}
    if(_Myhead != nullptr) {
        this->terminate();
        this->dispose();
    }
}

int thread_basic::activate(int flags, size_t stacksize)
{
    thread_info* info = new(std::nothrow) thread_info;
    if(nullptr == info) {
        return -1;
    }
    ::memset(info, 0x0, sizeof(thread_info));

    int stat = thread_basic::spawn((THREAD_PROC)thread_basic::_ThreadEntry, this, stacksize, flags, &info->handle);
    if(0 == stat)
    {
        this->_StorageThreadInfo(info);
    }
    else 
    {
        delete info;
    }
    return stat;
}

void thread_basic::activate_n(int n, int flags, size_t stacksize)
{
    for(int i = 0; i < n; ++i)
    {
        this->activate(flags, stacksize);
    }
}

int thread_basic::dispose(void)
{
    int count = 0;
#ifdef _WIN32
    thread_info* ptrdel = nullptr;
    thread_info* ptr = _Myhead;
    while(ptr != nullptr) {
        ::CloseHandle(ptr->handle);
        ptrdel = ptr;
        ptr = ptr->next;
        delete ptrdel;
        ++count;
    }
#endif
    _Myhead = nullptr;
    return count;
}

int thread_basic::wait(void)
{
    int count = 0;
    thread_info* ptr = _Myhead;
    while(ptr != nullptr) {
        thread_basic::join(ptr->handle);
        ptr = ptr->next;
        ++count;
    }
    return count;
}

int thread_basic::terminate(void)
{
    int count = 0;
    thread_info* ptr = _Myhead;
    while(ptr != nullptr) {
        thr_kill(ptr->handle);
        ptr = ptr->next;
        ++count;
    }
    return count;
}

void thread_basic::_StorageThreadInfo(thread_info* info)
{
    if(this->_Myhead != nullptr) {

        thread_info* ptr = _Myhead;
        while(ptr->next != nullptr)
        {
            ptr = ptr->next;
        }

        ptr->next = info;
    }
    else {
        this->_Myhead = info;
    }
}

long thread_basic::_ThreadEntry(thread_basic& object)
{
    return object.svc();
}

