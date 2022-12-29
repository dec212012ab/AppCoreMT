#ifndef __APP_CORE_HPP__
#define __APP_CORE_HPP__

#include <string>
#include <deque>
#include <unordered_map>
#include <thread>

#include "AppWorker.hpp"
#include "Version.hpp"

namespace AppCore{

class AppCoreMT{
public:
    struct WorkerObject{
        using Ptr = std::shared_ptr<WorkerObject>;
        AppWorker::Ptr worker;
        bool allow_task_exceptions=true;
        int task_priority_filter=-1;
    };

    AppCoreMT(size_t max_worker_count=0):max_worker_limit(max_worker_count){};
    ~AppCoreMT()=default;

    virtual void run();
    virtual void quit();

    bool addWorker();
    bool addWorker(std::string,bool=true,int=-1);
    bool endWorker(int);
    bool endWorker(std::string);

    bool assignWorkerTask(size_t,TaskChain::Ptr);
    bool assignWorkerTask(std::string,TaskChain::Ptr);
    //bool detachWorker();
    

    void setMaxWorkerLimit(size_t count);

    std::deque<WorkerObject::Ptr>::iterator begin(){return workers.begin();};
    std::deque<WorkerObject::Ptr>::iterator end(){return workers.end();};
    size_t size(){return workers.size();};
    WorkerObject::Ptr at(size_t index);
    WorkerObject::Ptr at(std::string name);

protected:
    std::deque<WorkerObject::Ptr> workers;
    size_t max_worker_limit;
};

}
#endif