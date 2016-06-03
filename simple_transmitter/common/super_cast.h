#ifndef _SUPER_CAST_H_
#define _SUPER_CAST_H_

namespace simplepp_1_0_2012_9 {
namespace super_cast {

template <typename _To, typename _From>
_To _uni_cast(_From value)
{
    //static_assert(sizeof(_From) == sizeof(_To), "invalid types for _uni_cast!");
    union
    {
        _From _from;
        _To   _to;
    } _Swaper;

    _Swaper._from = value;
    return _Swaper._to;
}

#ifndef _WIN64
#define _force_cast(dst, src)    \
    _asm push ebx                \
    _asm lea ebx, src            \
    _asm mov dword ptr[dst], ebx \
    _asm pop ebx

template<typename _Target, typename _Source> 
__declspec(naked) 
    _Target _asm_cast(_Source)
{
    __asm {
        mov eax, [esp + 4];
        ret;
    }
}
#endif

}; // namespace: simplepp_1_0_2012_9::super_cast
}; // namespace: simplepp_1_0_2012_9

#endif
/*
* Copyright (c) 2012-2019 by X.D. Guo ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
***/
