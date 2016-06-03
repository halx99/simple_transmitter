#ifndef _THREAD_SYNCH_H_
#define _THREAD_SYNCH_H_
#include "simpleppdef.h"
#include "thread_config.h"

namespace simplepp_1_0_2012_9 {
namespace asy {

class thread_cond;
class thread_mutex;
class thread_rwlock;
class semaphore;

#ifdef _WIN32
class mutex;
class event;
class waitable_timer;
#endif

/*
** WAIT_ABANDONED
** 0x00000080L The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated. Ownership of the mutex object is granted to the calling thread and the mutex state is set to nonsignaled.
** 
** If the mutex was protecting persistent state information, you should check it for consistency.
**  
** WAIT_OBJECT_0
** 0x00000000L The state of the specified object is signaled.
**  
** WAIT_TIMEOUT
** 0x00000102L The time-out interval elapsed, and the object's state is nonsignaled.
** 
** WAIT_FAILED
** (DWORD)0xFFFFFFFF The 
*/
typedef uint32_t wait_status;

// lockmode
#ifdef _WIN32
typedef long lockmode;
static const lockmode lm_shared = CONDITION_VARIABLE_LOCKMODE_SHARED; 
#endif

template<typename lock_type>
class scope_lock
{
public:
    scope_lock(lock_type& locker_ref) : locker(locker_ref)
    {
        this->locker.lock();
    }
    ~scope_lock(void)
    {
        this->locker.unlock();
    }

private:
    lock_type& locker;
};

class thread_mutex
{
    friend class thread_cond;
public:
    thread_mutex(void);
    ~thread_mutex(void);

    bool trylock(void);

    void lock(void);

    void unlock(void);

private:
    thread_mutex_t object;
};

class thread_rwlock
{
    friend class thread_cond;
public:
    thread_rwlock(void);
    ~thread_rwlock(void);

    void lock_shared(void);
    void unlock_shared(void);

    void lock_exclusive(void);
    void unlock_exclusive(void);

    bool trylock_shared(void);

    bool trylock_exclusive(void);

private:
    thread_rwlock_t object;
};

class thread_cond
{
public:
    thread_cond(void);
    ~thread_cond(void);

    void signal(void);
    void broadcast(void);

    bool wait(thread_mutex& cs);
    bool timedwait(thread_mutex& cs, long millseconds);

#ifdef _WIN32
    bool wait(thread_rwlock& rwl, lockmode = lm_shared);
    bool timedwait(thread_rwlock& rwl, long millseconds, lockmode = lm_shared);
#endif

private:
    thread_cond_t     object;
};


#ifdef _WIN32

class semaphore
{
public:
    semaphore(void);
    ~semaphore(void);

    bool open(LPCTSTR name);
    bool create(LPCTSTR name, int value = 0);
    
    wait_status wait(void);

    wait_status timedwait(DWORD millseconds);

    bool post(LONG number, LONG* previous_number = nullptr);

private:
    HANDLE object; // kernel object
};


class mutex
{
public:
    mutex(void);
    ~mutex(void);
    
    bool open(LPCTSTR name);
    bool create(LPCTSTR name);

    void lock(void);

    void unlock(void);

private:
    HANDLE object; // kernel object
};

class event
{
public:
    event(void);
    ~event(void);

    bool open(LPCTSTR name);
    bool create(LPCTSTR name);

    wait_status wait(void);

    wait_status timedwait(DWORD millseconds);

    void notify(void);

private:
    HANDLE object; // kernel object
};

class waitable_timer
{
public:
    waitable_timer(void);
    ~waitable_timer(void);

    bool open(LPCTSTR name);
    bool create(LPCTSTR name);

    bool set(const uint64_t duetime, LONG period, PTIMERAPCROUTINE completion_routine, LPVOID routine_arg, bool resume);

    bool cancel(void);

private:
    HANDLE object; // kernel object
};

#endif

}; // namespace: simplepp_1_0_2012_9::asy
}; // namespace: simplepp_1_0_2012_9

#endif

/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/


