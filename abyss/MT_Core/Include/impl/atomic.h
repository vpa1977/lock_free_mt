#pragma once

// defines basic atomic operations

struct atomic_long
{

	atomic_long() 
	{
		InterlockedExchange(&value, 0 );
	}
	atomic_long(long v) 
	{
		InterlockedExchange(&value, v );
	}

	inline long cas(volatile long val, long expected)
	{
		return InterlockedCompareExchange(&value, val, expected);
	}

	inline long get() const
	{
		volatile long tmp;
		InterlockedExchange(&tmp, value);
		return tmp;
	}
	inline void inc()
	{
		InterlockedIncrement(&value);
	}
	inline void dec()
	{
		InterlockedDecrement(&value);
	}
	inline void set(long val) 
	{
		volatile long tmp;
		tmp = val;
		InterlockedExchange(&value, tmp);
	}
private:	
	volatile long value;
};



