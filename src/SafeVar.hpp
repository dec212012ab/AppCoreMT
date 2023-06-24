#ifndef __SAFEVAR_HPP__
#define __SAFEVAR_HPP__

#include "SafeLock.hpp"

namespace AppCore{

template<typename T>
class SafeVar : public virtual ILockable{
public:
	using Ptr = std::shared_ptr<SafeVar<T>>;
	using UPtr= std::unique_ptr<SafeVar<T>>;

	SafeVar()=default;
	SafeVar(const SafeVar& other){
		_var = other._var;
	};
	SafeVar(const T& other){
		_var = other;
	};

	SafeVar(SafeVar&& other){
		_var = other._var;
	}

	SafeVar(T&& other){
		_var = other;
	}

	SafeVar& operator=(const SafeVar& other){
		_var = other._var;
		return *this;
	};
	SafeVar& operator=(const T& other){
		_var = other;
		return *this;
	};

	operator T&() {return _var;};
	operator T*() {return &_var;};

	virtual void lock() noexcept override{_lock.lock();};
	bool tryLock(size_t timeout_ms = 0){
		try{
			_lock.lockTimeout(timeout_ms);
			return _lock.isLocked();
		}
		catch(const SafeLockTimeoutException& e){
			return _lock.isLocked();
		}
	};
	virtual void unlock() override{
		_lock.unlock();
	};
	virtual bool isLocked() override{
		return _lock.isLocked();
	}

	T& get(){return _var;};
	T* getPtr(){return &_var;};

protected:
	friend bool operator==(const SafeVar<T>& lhs, const SafeVar<T>& rhs){return lhs._var == rhs._var;};
	friend bool operator==(const SafeVar<T>& lhs, const T& rhs){return lhs._var == rhs;};
	friend bool operator==(const T& lhs, const SafeVar<T>& rhs){return lhs == rhs._var;};

private:
	SafeLock _lock;
	T _var;
};

}
#endif