#include "APPCoreMT.hpp"

using namespace AppCore;

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

void AppCoreMT::run()
{
    for(auto& worker : workers){
        if(worker->worker){
            worker->worker->run(worker->allow_task_exceptions,worker->task_priority_filter);
        }
    }
}

void AppCoreMT::quit()
{
    for(auto& worker : workers){
        if(worker->worker){
            if(worker->worker->getRunning())worker->worker->stop();
        }
    }
}

bool AppCoreMT::addWorker()
{
    if(max_worker_limit>0 && workers.size()>= max_worker_limit)return false;
    WorkerObject::Ptr w = std::make_shared<WorkerObject>();
    w->worker = std::make_shared<AppWorker>();
    workers.emplace_back(w);
    return true;
}

bool AppCoreMT::addWorker(std::string name, bool allow_task_exceptions, int task_priority_filter)
{
    if(max_worker_limit>0 && workers.size()>= max_worker_limit)return false;
    WorkerObject::Ptr w = std::make_shared<WorkerObject>();
    w->worker = std::make_shared<AppWorker>(name);
    w->allow_task_exceptions = allow_task_exceptions;
    w->task_priority_filter = task_priority_filter;
    workers.emplace_back(w);
    return true;
}

bool AppCoreMT::endWorker(int index)
{
    if(index<0 || index>workers.size())return false;
    if(workers[index]->worker)workers[index]->worker->stop();
    return true;
}

bool AppCoreMT::endWorker(std::string name)
{
    WorkerObject::Ptr w = at(name);
    if(w){
        w->worker->stop();
        return true;
    }
    return false;
}

AppCoreMT::WorkerObject::Ptr AppCoreMT::at(size_t index)
{
    if(index<0 || index>= workers.size())return nullptr;
    return workers.at(index);
}

AppCoreMT::WorkerObject::Ptr AppCoreMT::at(std::string name)
{
    for(auto& w : workers){
        if(w && w->worker){
            if(w->worker->getWorkerName() == name)return w;
        }
    }
    return nullptr;
}

bool AppCoreMT::assignWorkerTask(size_t index, TaskChain::Ptr task)
{
    WorkerObject::Ptr w = at(index);
    if(!w)return false;
    if(!w->worker)return false;
    w->worker->enqueueTaskChain(task);
    return true;
}

bool AppCoreMT::assignWorkerTask(std::string name, TaskChain::Ptr task)
{
    WorkerObject::Ptr w = at(name);
    if(!w || !w->worker)return false;
    w->worker->enqueueTaskChain(task);
    return true;
}


#ifdef TESTING

SCENARIO("AppCoreMT Test","[AppCoreMT]"){
    GIVEN("An AppCoreMT instance"){
        AppCoreMT inst;
        WHEN("A Worker is Added"){
            WHEN("The worker is given a name"){
                inst.addWorker("TestWorker1");
                THEN("The worker stores the assigned name"){
                    REQUIRE(inst.at(0)->worker);
                    REQUIRE(inst.at(0)->worker->getWorkerName() == "TestWorker1");
                }
            }
            THEN("The worker list increases its size by one and does not run"){
                inst.addWorker();
                REQUIRE(inst.size()==1);
            }
            WHEN("A task chain is assigned and run"){
                int result = 18;
                TaskChain::Ptr chain = TaskChain::create();
                TaskNode::Ptr head = TaskNode::create<int, int>("Double Int", [](int a)->int {return 2 * a; }, &result, std::move(result));
                chain->addChainSegment(head);
                
                inst.addWorker("DoubleInt_Worker");
                inst.assignWorkerTask("DoubleInt_Worker", chain);
                inst.run();
                
                THEN("The output is correct"){
                    while (inst.at("DoubleInt_Worker")->worker->getTaskChainList().get().size() > 0) {
                        std::cout << "Waiting for pending tasks to complete..." << std::endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                    inst.quit();
                    REQUIRE(result == 36);
                }
            }
        }
    }
}

#endif