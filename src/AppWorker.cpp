#include "AppWorker.hpp"

using namespace AppCore;

size_t AppWorker::worker_id_gen = 0;

int AppWorker::workerMain(bool allow_task_exceptions, int task_priority_filter)
{
    if(_main)return _main();
    else{
        size_t tasks_pending = 0;
        while(!signal_stop){
            try{
                task_chain_list.lock();
                tasks_pending = task_chain_list.get().size();
                task_chain_list.unlock();

                if(tasks_pending<1){
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                else{
                    task_chain_list.lock();
                    TaskChain t = std::move(task_chain_list.get().front());
                    task_chain_list.get().pop_front();
                    task_chain_list.unlock();
                    t.executeChain(allow_task_exceptions,task_priority_filter);
                }
            }
            catch(const std::exception& e){
                std::cerr<<"Worker "<<worker_id<<": Executing Task Threw Exception!\n"<<e.what()<<std::endl;
            }
            catch(...){
                std::cerr<<"Worker "<<worker_id<<": Unknown Exception caught!"<<std::endl;
            }
        }
    }
}

void AppWorker::enqueueTaskChain(TaskChain&& t)
{
    task_chain_list.lock();
    task_chain_list.get().emplace_back(t);
    task_chain_list.unlock();
}

void AppWorker::enqueueTaskChain(TaskChain::Ptr p)
{
    task_chain_list.lock();
    task_chain_list.get().emplace_back(*p);
    task_chain_list.unlock();
}