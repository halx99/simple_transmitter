// The optimization of auto_ptr from standard library to support deleting object appropriately.
#ifndef _SIMPLE_PTR_H_
#define _SIMPLE_PTR_H_

namespace simplepp_1_0_2012_9 {

namespace gc {

// deletely function template
template<typename _Ty>
struct tfr
{
    typedef void (*fr)(_Ty*); 
};

// c++ single object deleter
template<typename _Ty>
void single_destroy(_Ty*); 

// c++ multiple objects deleter
template<typename _Ty>
void multiple_destroy(_Ty*); 

// pod type agile deleter
template<typename _Ty/*, typename tfr<_Ty>::fr*/>
void clgdef_destroy(_Ty*); 

// TEMPLATE CLASS simple_ptr
template< typename _Ty, typename tfr<_Ty>::fr = single_destroy<_Ty> >
class simple_ptr;

template<typename _Ty, typename tfr<_Ty>::fr _Destroy>
class simple_ptr
{    // wrap an object pointer to ensure destruction
public:
    typedef simple_ptr<_Ty, _Destroy> _Myt;
    typedef _Ty element_type;

    explicit simple_ptr(_Ty *_Ptr = 0) throw()
        : _Myptr(_Ptr)
    {    // construct from object pointer
    }

    simple_ptr(const _Myt& _Right) throw()
        : _Myptr(const_cast<_Myt&>(_Right).release())
    {    // construct by assuming pointer from _Right simple_ptr
    }

    _Myt& operator=(const _Myt& _Right) throw()
    {    // assign compatible _Right (assume pointer)
        reset(const_cast< _Myt& >(_Right).release());
        return (*this);
    }

    template<typename _T, typename tfr<_Ty>::fr _Dr>
    simple_ptr(const simple_ptr<_T, _Dr>& _Right) throw()
    {    // construct by assuming pointer from _Right
#ifdef __cxx11
        static_assert(is_same<_Deleter, _Dr>::value, "error, the deleting behavior must be consistent!");
#endif
        _Myptr = const_cast< simple_ptr<_T, _Dr>& >(_Right).release();
    }

    template<typename _T, typename tfr<_Ty>::fr _Dr>
    _Myt& operator=(const simple_ptr<_T, _Dr>& _Right) throw()
    {    // assign compatible _Right (assume pointer)
#ifdef __cxx11
        static_assert(is_same<_Deleter, _Dr>::value, "error, the deleting behavior must be consistent!");
#endif
        reset(const_cast< simple_ptr<_T, _Dr>& >(_Right).release());
        return (*this);
    }

    ~simple_ptr()
    {    // destroy the object
        _Destroy(_Myptr);
    }

    _Ty& operator*() const throw()
    {    // return designated value
        return (*_Myptr); // return (*get());
    }

    _Ty** operator &() throw()
    {
        return &(_Myptr);
    }

    _Ty *operator->() const throw()
    {    // return pointer to class object
        return (_Myptr); // return (get());
    }

    template<typename _Int>
    _Ty& operator[](_Int index) const throw()
    {
        return (_Myptr[index]);
    }

    _Ty* get() const throw()
    {    // return wrapped pointer
        return (_Myptr);
    }

    _Ty*& ref() throw()
    {    // return wrapped pointer
        return (_Myptr);
    }

    operator _Ty*() throw()
    {
        return (_Myptr);
    }

    _Ty *release() throw()
    {    // return wrapped pointer and give up ownership
        _Ty *_Tmp = _Myptr;
        _Myptr = 0;
        return (_Tmp);
    }

    void reset(_Ty *_Ptr = 0)
    {    // destroy designated object and store new pointer
        if (_Ptr != _Myptr) 
        { _Destroy(_Myptr); }
        _Myptr = _Ptr;
    }

private:
    _Ty *_Myptr;    // the wrapped object pointer
};

template<typename _Ty>
struct single_ptr // C++ single ptr
{
    typedef simple_ptr< _Ty, single_destroy<_Ty> > type;
};
template<typename _Ty>
struct multiple_ptr // C++ multiple ptr
{
    typedef simple_ptr< _Ty, multiple_destroy<_Ty> > type;
};

template<typename _Ty>
struct clgdef_ptr // C language default ptr
{
    typedef simple_ptr< _Ty, clgdef_destroy<_Ty> > type;
};

// c++ single object deleter
template<typename _Ty>
void single_destroy(_Ty* _Ptr)
{
    delete _Ptr;
}

// c++ multiple objects deleter
template<typename _Ty>
void multiple_destroy(_Ty* _Ptr)
{
    delete [] _Ptr;
}

// pod type agile deleter
template<typename _Ty/*, typename tfr<_Ty>::fr*/>
void clgdef_destroy(_Ty* _Ptr)
{
    free(_Ptr);
}

}; // namespace: simplepp_1_0_2012_9::gc
}; // namespace: simplepp_1_0_2012_9

#endif /* _SIMPLE_PTR_ */
/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
V3.0:2011 */

