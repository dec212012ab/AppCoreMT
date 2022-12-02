#ifndef __SAFEVAR_HPP__
#define __SAFEVAR_HPP__

#include "SafeLock.hpp"

template<typename T>
class SafeVar{
public:
	using Ptr = std::shared_ptr<SafeVar<T>>;
	using UPtr= std::unique_ptr<SafeVar<T>>;

	SafeVar()=delete;
	SafeVar(const SafeVar& other){
		_var = other._var;
	};
	SafeVar(const T& other){
		_var = other;
	};

	SafeVar& operator=(const SafeVar& other){
		_var = other._var;
	};
	SafeVar& operator=(const T& other){
		_var = other;
	};

	operator T&() {return _var;};

	void lock() noexcept{_lock.lock();};
	bool tryLock(size_t timeout_ms = 0){
		try{
			_lock.lockTimeout(ms);
			return true;
		}
		catch(const SafeLockTimeoutException& e){
			return false;
		}
	};
	void unlock(){
		_lock.unlock();
	};

	T& get(){return _var;};

private:
	SafeLock _lock;
	T _var;
};

#endif