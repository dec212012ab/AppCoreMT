#include "AppWorker.hpp"

//#define TESTING

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

using namespace AppCore;

size_t AppWorker::worker_id_gen = 0;

int AppWorker::workerMain(bool allow_task_exceptions, int task_priority_filter)
{
    int retcode = 0;
    running.lock();
    running=true;
    running.unlock();
    if(_main){
        retcode = _main(worker_id,task_chain_list, allow_task_exceptions, task_priority_filter);
    }
    else{
        size_t tasks_pending = 0;
        while(!this->signal_stop){
            try{
                task_chain_list.lock();
                tasks_pending = task_chain_list.get().size();
                task_chain_list.unlock();

                if(tasks_pending<1){
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                else{
                    task_chain_list.lock();
                    TaskChain::Ptr t = std::move(task_chain_list.get().front());
                    task_chain_list.get().pop_front();
                    task_chain_list.unlock();
                    if(!t->executeChain(allow_task_exceptions,task_priority_filter)){
                        std::cerr<<"Worker: "<<worker_name<<": Task Chain execution returned false!"<<std::endl;
                    }
                }
            }
            catch(const std::exception& e){
                std::cerr<<"Worker: "<<worker_name<<": Executing Task Threw Exception!\n"<<e.what()<<std::endl;
                retcode++;
            }
            catch(...){
                std::cerr<<"Worker: "<<worker_name<<": Unknown Exception caught!"<<std::endl;
                retcode++;
            }
        }
    }
    running.lock();
    running=false;
    running.unlock();
    return retcode;
}

void AppWorker::enqueueTaskChain(TaskChain::Ptr p)
{
    task_chain_list.lock();
    task_chain_list.get().emplace_back(p);
    task_chain_list.unlock();
}

void AppWorker::run(bool allow_task_exceptions, int task_priority_filter)
{
    bool is_running = false;
    running.lock();
    is_running = running;
    running.unlock();
    if(is_running)return;
    else{
        if(_worker){
            stop();
        }
        signal_stop = false;
        _worker = std::make_shared<std::thread>(&AppWorker::workerMain,this,allow_task_exceptions,task_priority_filter);
        while (!running) {
            //std::cout << "Waiting for worker " << worker_id << " to start..." << std::endl;
        }
        //std::cout << "Worker " << worker_id << " started!" << std::endl;
    }
}

void AppWorker::stop()
{
    if (!_worker)return;

    bool is_running = false;
    running.lock();
    is_running = running;
    running.unlock();
    if(is_running){
        do{
            signal_stop = true;
            running.lock();
            is_running = running;
            running.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } while (is_running);

    }
    else {
        throw std::runtime_error("AppWorker::stop() : Thread was not actually running!");
    }
    
    if(_worker->joinable())_worker->join();
    _worker=nullptr;
    
}


#ifdef TESTING // AppWorker

SCENARIO("AppWorker Construction Tests","[AppWorker][AppWorker::Construction]"){
    GIVEN("Two AppWorker instances"){
        AppWorker a,b;
        THEN("The second worker's ID is one more than the first"){
            REQUIRE(a.getWorkerID()+1 == b.getWorkerID());
        }
    }
    GIVEN("A newly created AppWorker Instance"){
        AppWorker a;
        THEN("The running and stop flags are unset"){
            REQUIRE(!a.getRunning());
            REQUIRE(!a.getStopFlag());
        }
    }
};

SCENARIO("AppWorker Task Queue Tests","[AppWorker][AppWorker::TaskQueue]"){
    GIVEN("An AppWorker Instance"){
        WHEN("A task chain is created and enqueued"){
            AppWorker worker;
            TaskChain::Ptr chain = std::make_shared<TaskChain>();
            TaskNode::Ptr head = std::make_shared<TaskNode>();
            int result = 7;

            head = TaskNode::create<int, int>("Double Int", [](int a)->int {return 2 * a; }, &result, std::move(result));
            head->setNextNode(TaskNode::create<int, int>("Triple Int", [](int a)->int {return 3 * a; }, &result, std::move(result)));



            //head->setTask(AppTask<int,int*>::create("Double int",[](int* a)->int{return 2*(*a);}),&result,&input);
            //head->setNextNode(TaskNode::create<int,int*>("Triple Number",[](int* a)->int{return 3*(*a);},&result,&result));
            chain->addChainSegment(head);
            worker.enqueueTaskChain(chain);
            THEN("The task chain queue's size increases by one and the task chain does not execute."){
                REQUIRE(!worker.getRunning());
                REQUIRE(!worker.getStopFlag());
                REQUIRE(worker.getTaskChainList().get().size()==1);
            }
            
            WHEN("The worker is run, the result is correctly returned") {
                worker.run(false);
                while (worker.getTaskChainList().get().size() > 0) {
                    std::cout << "Waiting for pending tasks to complete..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                worker.stop();
                REQUIRE(result == 42);
            }
        }
    }
};

#endif
