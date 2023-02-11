#ifndef __APPTASK_HPP__
#define __APPTASK_HPP__

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <typeinfo>

namespace AppCore{

/**
 * @class AppTask
 * 
 * @tparam R : Return type
 * @tparam Args : Packed Argument types
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

	/**
	 * @brief Construct a new App Task object
	 * 
	 */
	AppTask(){
		//std::cout<<typeid(R).name() << std::endl;
	};

	/**
	 * @brief Destroy the App Task object
	 * 
	 */
	~AppTask()=default;

	/**
	 * @brief Set the Task Function object
	 * 
	 * @param task_name : Name of the task
	 * @param fn : The function object defining the task
	 */
	void setTaskFunction(std::string task_name,std::function<R(Args...)> fn){task_id = task_name;_task = fn;};

	/**
	 * @brief Get the Task Function object
	 * 
	 * @return std::function<R(Args...)>& 
	 */
	std::function<R(Args...)>& getTaskFunction(){return _task;};

	/**
	 * @brief Static task creation function. Useful at the top level
	 * when creating task chains.
	 * 
	 * @param task_name : Name of the task
	 * @param fn : The function object defining the task
	 * @return std::shared_ptr<AppTask<R,Args...>> 
	 */
	static std::shared_ptr<AppTask<R,Args...>> create(std::string task_name, std::function<R(Args...)> fn) {
		std::shared_ptr<AppTask<R,Args...>> t = std::make_shared<AppTask<R, Args...>>();
		t->setTaskFunction(task_name, fn);
		return t;
	};

	

	/**
	 * @brief Set the Allow Retry flag
	 * @note Only valid if allow_exceptions is disabled
	 * @param b : New state of the flag
	 */
	void setAllowRetry(bool b){allow_retries = b;};

	/**
	 * @brief Get the Allow Retry flag
	 * 
	 * @return bool
	 */
	bool getAllowRetry(){return allow_retries;};

	/**
	 * @brief Set the Max Retries count used to determine
	 * how many times a task should be re-run on a crash before
	 * dropping it and reporting an error.
	 * 
	 * @param max_retry : New retry count
	 */
	void setMaxRetries(uint32_t max_retry){max_retries = max_retry;};
	
	/**
	 * @brief Get the Max Retries count
	 * 
	 * @return uint32_t 
	 */
	uint32_t getMaxRetries(){return max_retries;};

	/**
	 * @brief Set the Allow Exceptions flag. If enabled
	 * then the task will not attempt to internally handle
	 * thrown exceptions.
	 * 
	 * @param b : New state of the allow_exceptions flag
	 */
	void setAllowExceptions(bool b){allow_exceptions = b;};

	/**
	 * @brief Get the Allow Exceptions flag
	 * 
	 * @return bool
	 */
	bool getAllowExceptions(){return allow_exceptions;};

	/**
	 * @brief Get the Task ID (i.e task name)
	 * 
	 * @return std::string 
	 */
	std::string getTaskID() { return task_id; }

#ifdef TESTING
	uint32_t getRetryCounter(){return retries;};
#endif

	/**
	 * @brief Run the stored task function with the provided
	 * arguments.
	 * 
	 * @param args : Packed list of arguments
	 * @return std::optional<R> 
	 */
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

	/**
	 * @brief Convenience operator overload which invoked the run() function. 
	 * 
	 * @param args : Packed list of arguments
	 * @return std::optional<R> 
	 */
	std::optional<R> operator() (Args&& ... args) { return run(args...); };

private:
	std::string task_id;///< The name or id of the task
	std::function<R(Args...)> _task; ///< The task function
	uint32_t retries = 0; ///< The current number of retries attempted
	uint32_t max_retries = 0; ///< The maximum number of retries allowed
	bool allow_retries = false;///< Flag indicating if retries are allowed on task failure
	bool allow_exceptions = true; ///<Flag indicating if running the task is allowed to throw exceptions or if any thrown exceptions will be handled internally
};

}
#endif
