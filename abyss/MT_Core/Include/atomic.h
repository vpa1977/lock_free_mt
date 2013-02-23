#ifndef ATOMIC_HPP_INCLUDED
#define ATOMIC_HPP_INCLUDED

#include <assert.h>

namespace atomic
{

template<typename T>
T Exchange(volatile T* dest, T new_value )
{
	assert(dest);
	T comparand;
	do 
	{
		comparand = *dest;
	} while( !SetIfEqual(dest, new_value, comparand) );
	return comparand;
}

template<typename T>
T ExchangeAdd(volatile T* dest, T value )
{
	assert(dest);
	T comparand;
	do 
	{
		comparand = *dest;
	} while( !SetIfEqual(dest, comparand+value, comparand) );
	return comparand;
}

template<typename T>
bool SetIfEqual(volatile T* dest, T exchange, T comparand )
{
	assert(dest);
    __asm 
    {
        lea esi, comparand;
        lea edi, exchange;
        mov eax, [esi];
        mov edx, 4[esi];
        mov ebx, [edi];
        mov ecx, 4[edi];
        mov esi, dest;
        lock CMPXCHG8B [esi];
        mov eax, 0;
        setz al;
    }
}

} // namespace atomic

#endif
