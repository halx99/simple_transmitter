/**** Notes:
*  When the object hold by thread will be destroyed , 
*  the thread must already exit or no longer dependent on any 
*  resources of the object.
*/
#ifndef _THREAD_BASIC_H_
#define _THREAD_BASIC_H_
#include "thread_config.h"

namespace simplepp_1_0_2012_9 { 

namespace asy {

#ifdef _WIN32
typedef DWORD thrid_t;
typedef HANDLE thread_t;
typedef DWORD (__stdcall*THREAD_PROC)(void*);
#else
typedef pthread_t thrid_t;
typedef pthread_t thread_t;
typedef void* (*THREAD_PROC)(void*);
#endif

#ifdef __linux
enum {
    thr_joinable = PTHREAD_CREATE_JOINABLE,
    thr_detached = PTHREAD_CREATE_DETACHED
};
#endif


struct thread_info
{
    thread_t     handle;
    thread_info* next;
};

class thread_basic
{
public:
    /*
    ** @brief  : Creates a thread to execute within the virtual address space of the calling process.
    ** @params : 
    **       1st: A pointer to the application-defined function to be executed by the thread. This pointer 
    **            represents the starting address of the thread. For more information on the thread function,
    **            see THREAD_PROC. 
    **       2nd: A pointer to a variable to be passed to the thread.
    **       3rd: A pointer to a variable that receives the thread identifier. If this parameter is NULL, 
    **            the thread identifier is not returned.
    **       4th: The flags that control the creation of the thread.
    **            Windows NT: ignore this parameter
    **            Linux     : thr_joinable or thr_detached
    ** @returns: 
    **       Windows NT: Please checkout reason using VC++ Menu [Tools]-->[Error Lookup]
    **       Linux     : EAGAIN(11)-->System limit new thread being created
    **                   EINVAL(22)-->The attr is illegal which is set by the 2nd parameter
    */
    static int     spawn(THREAD_PROC, void* arg = nullptr,  size_t stacksize = 0, int flags = 0, thread_t* pthr = nullptr);
    static int     join(thread_t& thr);
    static int     join_n(thread_t* thrs, size_t number);

public:
    thread_basic(void);
    virtual ~thread_basic(void);

    int            activate(int flags = 0, size_t stacksize = 0);

    void           activate_n(int n, int flags = 0, size_t stacksize = 0);

    int            dispose(void);

    int            wait(void);

    int            terminate(void);

private:
    virtual long   svc(void) = 0;

    static long    _ThreadEntry(thread_basic&);

    void           _StorageThreadInfo(thread_info*);
private:
    thread_info*   _Myhead;
};

}; // namespace: simplepp_1_0_2012_9::asy
}; // namespace: simplepp_1_0_2012_9

#endif
/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/

