#ifndef __SAFELOCK_HPP__
#define __SAFELOCK_HPP__

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>

class SafeLockTimeoutException : public std::exception{
public:
	char* what(){
		return "SafeLock Timeout Exception";
	}
};

class SafeLock{
public:
	SafeLock()=default;
	~SafeLock();
	void lock() noexcept;
	void lockTimeout(size_t timeout_ms=0);
	void unlock();
	bool isLocked(){return is_locked!=0;};
#ifdef TESTING
	int getIsLockedCounter(){return is_locked;}
#endif
	operator std::recursive_timed_mutex&(){return _internal;};
private:
	int is_locked = 0;
	std::recursive_timed_mutex _internal;
};


#endif
