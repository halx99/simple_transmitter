// container_helper standard header, contains several methods to help clear dynamic store of std-container.
#ifndef _CONTAINER_HELPER_H_
#define _CONTAINER_HELPER_H_
#include <algorithm>

namespace simplepp_1_0_2012_9 {

namespace container_helper {

/// <destroy all object of container>
template<typename _C> inline
    void clear_c(_C& _Obj)
{
    typename _C::iterator _First = _Obj.begin();
    typename _C::iterator _Last = _Obj.end();
    for (; _First != _Last; ++_First) {
        delete *_First;
    }
    _Obj.clear();
}

template<typename _CC> inline
    void clear_cc(_CC& _Obj)
{
    typename _CC::iterator _First = _Obj.begin();
    typename _CC::iterator _Last = _Obj.end();
    for (; _First != _Last; ++_First) {
        delete _First->second;
    }
    _Obj.clear();
}

template<typename _InIt, typename _Fn1> inline
    _Fn1 for_each2(_InIt _First, _InIt _Last, _Fn1 _Func)
{    // perform function for each 2nd element
    for (; _First != _Last; ++_First)
        _Func(_First->second);
    return (_Func);
}

template<typename _Container> inline
void push_back_to(const typename _Container::value_type& val, _Container& container)
{
    container.push_back(val);
}

template<typename _Container> inline
void insert_into(const typename _Container::value_type& val, _Container& container)
{
    container.insert(val);
}

}; // namespace: simplepp_1_0_2012_9::container_helper
}; // namespace: simplepp_1_0_2012_9

#endif
/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/

