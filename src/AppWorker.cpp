#include "AppWorker.hpp"

size_t AppWorker::worker_id_gen = 0;

int AppWorker::workerMain()
{
    if(_main)return _main();
    else{
        while(!signal_stop){
            try{

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