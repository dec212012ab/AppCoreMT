#ifndef __APPWORKER_HPP__
#define __APPWORKER_HPP__

#include <functional>
#include <iostream>

namespace AppCore{

class AppWorker{
public:
	AppWorker():worker_id(AppWorker::worker_id_gen++){};
	~AppWorker()=default;

	int workerMain();

	void setCustomWorkerMain(std::function<int(void)> fn){_main=fn;};
	
	bool getRunning(){return running;};
	bool getStopFlag(){return signal_stop;};
	size_t getWorkerID(){return worker_id;};

	void setStopFlag(bool b){signal_stop = b;};

private:
	static size_t worker_id_gen;
	size_t worker_id;
	bool running=false;
	bool signal_stop = false;
	std::function<int(void)> _main;
};

}
#endif
