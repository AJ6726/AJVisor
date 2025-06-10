#pragma once
#include <intrin.h>

//https://github.com/jonomango/hv/blob/5f56620050fe4d6f516ce9ff65f8d64ec90cae4c/hv/spin-lock.h#L26
struct spinlock
{
	void initialize() 
	{
		lock = 0;
	}

	void acquire() 
	{
		while (1 == _InterlockedCompareExchange(&lock, 1, 0))
			_mm_pause();
	}

	void release()
	{
		lock = 0;
	}

	volatile long lock;
};

class scoped_spinlock 
{
public:
	scoped_spinlock(spinlock& lock) : lock_(lock) 
	{
		lock.acquire();
	}

	~scoped_spinlock() 
	{
		lock_.release();
	}

	scoped_spinlock(scoped_spinlock const&) = delete;
	scoped_spinlock& operator=(scoped_spinlock const&) = delete;

private:
	spinlock& lock_;
};


