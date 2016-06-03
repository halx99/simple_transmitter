#include "thread_synch.h"
#ifdef _WIN32
#pragma warning(disable:4800)
#else
#include <time.h>
#endif

using namespace simplepp_1_0_2012_9::asy;

// thread_mutex
thread_mutex::thread_mutex(void)
{
    thread_mutex_init(&this->object, nullptr);
}
thread_mutex::~thread_mutex(void)
{
    thread_mutex_destroy(&this->object);
}

void thread_mutex::lock(void)
{
    thread_mutex_lock(&this->object);
}

bool thread_mutex::trylock(void)
{
    return thread_mutex_trylock(&this->object);
}

void thread_mutex::unlock(void)
{
    thread_mutex_unlock(&this->object);
}


// thread_rwlock
thread_rwlock::thread_rwlock(void)
{
    thread_rwlock_init(&this->object, nullptr);
}
thread_rwlock::~thread_rwlock(void)
{
    thread_rwlock_destroy(&this->object);
}

void thread_rwlock::lock_shared(void)
{
    thread_rwlock_lock_shared(&this->object);
}

void thread_rwlock::unlock_shared(void)
{
    thread_rwlock_unlock_shared(&this->object);
}

void thread_rwlock::lock_exclusive(void)
{
    thread_rwlock_lock_exclusive(&this->object);
}

void thread_rwlock::unlock_exclusive(void)
{
    thread_rwlock_unlock_exclusive(&this->object);
}

bool thread_rwlock::trylock_shared(void)
{
    return thread_rwlock_trylock_shared(&this->object);
}

bool thread_rwlock::trylock_exclusive(void)
{
    return thread_rwlock_trylock_exclusive(&this->object);
}

// thread_cond
thread_cond::thread_cond(void)
{
    thread_cond_init(&this->object, nullptr);
}

thread_cond::~thread_cond(void)
{
    thread_cond_destroy(&this->object);
}

void thread_cond::signal(void)
{
    thread_cond_signal(&this->object);
}

void thread_cond::broadcast(void)
{
    thread_cond_broadcast(&this->object);
}

bool thread_cond::wait(thread_mutex& mtx)
{
    return thread_cond_wait(&this->object, &mtx.object);
}

bool thread_cond::timedwait(thread_mutex& mtx, long millseconds)
{
#ifdef _WIN32
    return ::SleepConditionVariableCS(&this->object, &mtx.object, millseconds);
#else
    timespec timeo;
    timeo.tv_sec = millseconds / 1000;
    timeo.tv_nsec = (millseconds - (timeo.tv_sec * 1000)) * 1000000;
    return pthread_cond_timedwait(&this->object, &mtx.object, &timeo);
#endif
}

#ifdef _WIN32
bool thread_cond::wait(thread_rwlock& rwl, lockmode mode)
{
    return ::SleepConditionVariableSRW(&this->object, &rwl.object, INFINITE, mode);
}

bool thread_cond::timedwait(thread_rwlock& rwl, long millseconds, lockmode mode)
{
    return ::SleepConditionVariableSRW(&this->object, &rwl.object, millseconds, mode);
}
#endif

#ifdef _WIN32
// semaphore
semaphore::semaphore(void) : object(nullptr)
{
}

semaphore::~semaphore(void)
{
    if(this->object != nullptr)
    {
        ::CloseHandle(this->object);
    }
}

bool semaphore::open(LPCTSTR name)
{
    return (this->object = ::OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, name)) != nullptr;
}

bool semaphore::create(LPCTSTR name, int value)
{
    return (this->object = ::CreateSemaphore(nullptr, value, INFINITE, name)) != nullptr;
}

wait_status semaphore::wait(void)
{
    return ::WaitForSingleObject(this->object, INFINITE);
}

wait_status semaphore::timedwait(DWORD millseconds)
{
    return ::WaitForSingleObject(this->object, millseconds);
}

bool semaphore::post(LONG number, LONG* previous_number)
{
    if(previous_number != nullptr) {
        return ::ReleaseSemaphore(this->object, number, previous_number);
    }
    return ::ReleaseSemaphore(this->object, number, nullptr) ;
}

// mutex
mutex::mutex(void) : object(nullptr)
{
}

mutex::~mutex(void)
{
    if(this->object != nullptr)
    {
        ::CloseHandle(this->object);
    }
}

bool mutex::open(LPCTSTR name)
{
    return (this->object = ::OpenEvent(MUTEX_ALL_ACCESS, FALSE, name)) != nullptr;
}

bool mutex::create(LPCTSTR name)
{
    return (this->object = ::CreateMutex(nullptr, FALSE, name)) != nullptr;
}

void mutex::lock(void)
{
    (void)::WaitForSingleObject(object, INFINITE);
}

void mutex::unlock(void)
{
    (void)::ReleaseMutex(this->object);
}

// event
event::event(void) : object(nullptr)
{
}

event::~event(void)
{
    if(this->object != nullptr)
    {
        ::CloseHandle(this->object);
    }
}

bool event::open(LPCTSTR name)
{
    return (this->object = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, name)) != nullptr;
}

bool event::create(LPCTSTR name)
{
    return (this->object = ::CreateEvent(nullptr, FALSE, FALSE, name)) != nullptr;
}

wait_status event::wait(void)
{
    return ::WaitForSingleObject(object, INFINITE);
}

wait_status event::timedwait(DWORD millseconds)
{
    return ::WaitForSingleObject(object, millseconds);
}

void event::notify(void)
{
    (void)::SetEvent(object);
}

// waitable_timer
waitable_timer::waitable_timer(void) : object(nullptr)
{
}

waitable_timer::~waitable_timer(void)
{
    if(this->object != nullptr)
    {
        ::CloseHandle(this->object);
    }
}

bool waitable_timer::open(LPCTSTR name)
{
    return (this->object = ::OpenWaitableTimer(TIMER_ALL_ACCESS, FALSE, name)) != nullptr; 
}

bool waitable_timer::create(LPCTSTR name)
{
    return (this->object = ::CreateWaitableTimer(nullptr, FALSE, name)) != nullptr; 
}

bool waitable_timer::set(const uint64_t duetime, LONG period, PTIMERAPCROUTINE completion_routine, LPVOID routine_arg, bool resume)
{
    return ::SetWaitableTimer(this->object, (LARGE_INTEGER*)&duetime, period, completion_routine, routine_arg, resume) ;
}

bool waitable_timer::cancel(void)
{
    return CancelWaitableTimer(this->object);
}

#endif

