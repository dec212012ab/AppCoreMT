#ifndef __SAFELOCK_HPP__
#define __SAFELOCK_HPP__

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>

namespace AppCore{

class ILockable{
public:
	virtual ~ILockable() = default;
	virtual void lock() noexcept = 0;
	virtual void unlock() = 0;
	virtual bool isLocked() = 0;
};

class SafeLockTimeoutException : public std::exception{
public:
	char* what(){
		return "SafeLock Timeout Exception";
	}
};

class SafeLock : public virtual ILockable{
public:
	SafeLock()=default;
	~SafeLock();
	virtual void lock() noexcept override;
	void lockTimeout(size_t timeout_ms=0);
	virtual void unlock();
	virtual bool isLocked() override{return is_locked!=0;};
#ifdef TESTING
	int getIsLockedCounter(){return is_locked;}
#endif
	operator std::recursive_timed_mutex&(){return _internal;};
private:
	std::atomic<int> is_locked = 0;
	std::recursive_timed_mutex _internal;
};

class ScopedLock{
public:
	ScopedLock(ILockable& lock):lock_ref(lock)
	{
		lock_ref.lock();
	}
	~ScopedLock()
	{
		lock_ref.unlock();
	}
	bool isLocked(){return lock_ref.isLocked();};
private:
	ILockable& lock_ref = SafeLock();
};

}

#endif
