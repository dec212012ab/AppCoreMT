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

	AppWorker():worker_id(AppWorker::worker_id_gen++){
		std::stringstream ss;
		ss<<worker_id;
		worker_name = "Worker-"+ss.str();
	};
	AppWorker(std::string name):worker_id(AppWorker::worker_id_gen++),worker_name(name){};
	~AppWorker()=default;

	int workerMain(bool allow_task_exceptions, int task_priority_filter=-1);

	void setCustomWorkerMain(std::function<int(size_t,SafeVar<std::list<TaskChain::Ptr>>&,bool,int)> fn){_main=fn;};
	
	bool getRunning(){
		running.lock();
		bool ret = running;
		running.unlock();
		return ret;
	};

	bool getStopFlag(){return signal_stop;};
	size_t getWorkerID(){return worker_id;};
	SafeVar<std::list<TaskChain::Ptr>>& getTaskChainList(){return task_chain_list;};	

	std::string getWorkerName(){return worker_name;};

	void enqueueTaskChain(TaskChain::Ptr p);
	//void enqueueTaskChain(TaskChain&& t);

	void run(bool allow_task_exceptions=true, int task_priority_filter=-1);
	void stop();

private:
	static size_t worker_id_gen;
	size_t worker_id;
	std::string worker_name;
	SafeVar<bool> running=false;
	bool run_finished = false;
	bool signal_stop = false;
	std::function<int(size_t,SafeVar<std::list<TaskChain::Ptr>>&,bool,int)> _main;
	SafeVar<std::list<TaskChain::Ptr>> task_chain_list;
	std::shared_ptr<std::thread> _worker;
};

}
#endif
