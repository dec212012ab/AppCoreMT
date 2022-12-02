#ifndef __APPTASK_HPP__
#define __APPTASK_HPP__

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <typeinfo>


/**
 * @class AppTask
 * 
 * @tparam R 
 * @tparam Args 
 * 
 * @brief Wrapper class that handles individual task routines.
 * 
 * @details This class wraps around std::function but provides additional
 * logic to allow or block exceptions, retrieve return values as applicable,
 * or allow for a certain number of retries in the event that the provided
 * task function fails to execute properly.
 */
template<typename R, typename... Args>
class AppTask{
public:
	using Ptr = std::shared_ptr<AppTask>;
	using UPtr= std::unique_ptr<AppTask>;

	AppTask(){
		//std::cout<<typeid(R).name() << std::endl;
	};
	~AppTask()=default;

	void setTaskFunction(std::string task_name,std::function<R(Args...)> fn){task_id = task_name;_task = fn;};
	std::function<R(Args...)>& getTaskFunction(){return _task;};

	//template<typename R2, typename... Args2>
	static std::shared_ptr<AppTask<R,Args...>> create(std::string task_name, std::function<R(Args...)> fn) {
		std::shared_ptr<AppTask<R,Args...>> t = std::make_shared<AppTask<R, Args...>>();
		t->setTaskFunction(task_name, fn);
		return t;
	};

	void setAllowRetry(bool b){allow_retries = b;};//Only valid if allow_exceptions is disabled
	bool getAllowRetry(){return allow_retries;};

	void setMaxRetries(uint32_t max_retry){max_retries = max_retry;};
	uint32_t getMaxRetries(){return max_retries;};

	void setAllowExceptions(bool b){allow_exceptions = b;};
	bool getAllowExceptions(){return allow_exceptions;};

	std::string getTaskID() { return task_id; }

#ifdef TESTING
	uint32_t getRetryCounter(){return retries;};
#endif

	std::optional<R> run(Args&&... args){
		if(!_task)return std::nullopt;
		bool success = false;
		retries = 0;
		do{
			if(allow_exceptions)
				return _task(std::forward<Args>(args)...);
			else{
				try{
					return _task(std::forward<Args>(args)...);
				}
				catch(const std::exception& e){
					if(allow_retries && retries<max_retries){
						std::cerr<<"AppTask (id: "<<task_id<<") failed. (Retry Count: "<<++retries<<")" << std::endl;
						continue;
					}
					std::cerr<<"AppTask (id: "<<task_id<<") Triggered Exception: "<<e.what()<<std::endl;
					return std::nullopt;
				}
				catch(...){
					if(allow_retries && retries<max_retries){
						std::cerr<<"AppTask (id: "<<task_id<<") failed with unknown error. (Retry Count: "<<++retries<<std::endl;
						continue;
					}
					std::cerr<<"AppTask (id: "<<task_id<<") Triggered Generic Exception!"<<std::endl;
					return std::nullopt;
				}
			}
		}while(!success && allow_retries && retries<max_retries);
		return std::nullopt;
	}

	std::optional<R> operator() (Args&& ... args) { return run(args...); };

private:
	std::string task_id;
	std::function<R(Args...)> _task;
	uint32_t retries = 0;
	uint32_t max_retries = 0;
	bool allow_retries = false;
	bool allow_exceptions = true;
};
#endif
