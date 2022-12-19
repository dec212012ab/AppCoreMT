#ifndef __APPWORKER_HPP__
#define __APPWORKER_HPP__

#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include "SafeVar.hpp"
#include "TaskChain.hpp"

namespace AppCore{

class AppWorker{
public:
	using Ptr = std::shared_ptr<AppWorker>;

	AppWorker():worker_id(AppWorker::worker_id_gen++){};
	~AppWorker()=default;

	int workerMain(bool allow_task_exceptions, int task_priority_filter=-1);

	void setCustomWorkerMain(std::function<int(size_t,SafeVar<std::list<TaskChain>>&,bool,int)> fn){_main=fn;};
	
	bool getRunning(){
		running.lock();
		bool ret = running;
		running.unlock();
		return ret;
	};

	bool getStopFlag(){return signal_stop;};
	size_t getWorkerID(){return worker_id;};
	SafeVar<std::list<TaskChain>>& getTaskChainList(){return task_chain_list;};	

	void enqueueTaskChain(TaskChain::Ptr p);
	void enqueueTaskChain(TaskChain&& t);

	void run(bool allow_task_exceptions=true, int task_priority_filter=-1);
	void stop();

private:
	static size_t worker_id_gen;
	size_t worker_id;
	SafeVar<bool> running=false;
	bool run_finished = false;
	bool signal_stop = false;
	std::function<int(size_t,SafeVar<std::list<TaskChain>>&,bool,int)> _main;
	SafeVar<std::list<TaskChain>> task_chain_list;
	std::shared_ptr<std::thread> _worker;
};

}
#endif
