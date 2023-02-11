#ifndef __APP_CORE_HPP__
#define __APP_CORE_HPP__

#include <string>
#include <deque>
#include <unordered_map>
#include <thread>

#include "AppWorker.hpp"
#include "DynamicLibLoader.hpp"
#include "ResourceProvisioner.hpp"

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

    AppCoreMT(size_t max_worker_count=0):max_worker_limit(max_worker_count){
        shared_resources = ResourceProvisioner::create();
    };
    ~AppCoreMT()=default;

    virtual void run();
    virtual void quit();

    bool addWorker();
    bool addWorker(std::string name,bool allow_task_exceptions=true,int task_priority_filter=-1);
    bool endWorker(int index);
    bool endWorker(std::string name);

    bool assignWorkerTask(size_t index,TaskChain::Ptr task);
    bool assignWorkerTask(std::string name,TaskChain::Ptr task);
    //bool detachWorker();
    

    void setMaxWorkerLimit(size_t count);

    std::deque<WorkerObject::Ptr>::iterator begin(){return workers.begin();};
    std::deque<WorkerObject::Ptr>::iterator end(){return workers.end();};
    size_t size(){return workers.size();};
    WorkerObject::Ptr at(size_t index);
    WorkerObject::Ptr at(std::string name);
    ResourceProvisioner::Ptr getSharedProvisioner(){return shared_resources;};

protected:
    std::deque<WorkerObject::Ptr> workers;
    size_t max_worker_limit;
    ResourceProvisioner::Ptr shared_resources;
};

}
#endif