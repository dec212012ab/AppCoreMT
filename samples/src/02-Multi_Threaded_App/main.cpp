
#include <chrono>
#include <sstream>
#include "AppCoreMT.hpp"

using namespace AppCore;

int main()
{
    AppCoreMT app;

    //Shared Resources

    //First set the queue limit for the shared queue that will be used to pass
    // data from the producer to the consumer
    app.getSharedProvisioner()->addSharedResource("common","queue_limit",5);

    //Create the queue used to pass data between the producer and consumer
    app.getSharedProvisioner()->addSharedResource("common","queue",std::vector<int>());

    //Create a shared flag that will tell the consumer that it can exit after 
    // consuming whatever is left in the shared queue
    app.getSharedProvisioner()->addSharedResource("common","producer_complete",false);


    //Build chains for the producer and consumer
    TaskChain::Ptr producer = TaskChain::create();
    TaskChain::Ptr consumer = TaskChain::create();

    /* Create the producer thread's main function as a TaskNode and
     * assign it to the producer's task chain. Note that as a limitation
     * of the TaskNode construction, the return type cannot be void as 
     * otherwise the compiler will complain about the template type parameters
     * being incorrect. For that reason, the third variant of TaskNode::create
     * is used to create a task node that returns an int but does not take any
     * parameters at execution time.
     */
    producer->addChainSegment(
        TaskNode::create<int>(std::string("producer_task"),
            [provisioner=app.getSharedProvisioner()]()->int{
                int value = 0; //Variable treated as the data and as a loop control variable

                //Get the maximum size of the queue from the shared resource provisioner
                auto max_size = provisioner->getSharedResource("common","queue_limit")->safeGetAs<int>(true);
                
                //If the max size of the shared queue could not be obtained, use 5 as the default
                if(!max_size){
                    std::cerr<<"Producer Task: Failed to get Max Size! Using 5...\n";
                    max_size = 5;
                }

                //Get the SharedResource pointers for the completion flag and the shared queue
                auto complete_flag = provisioner->safeGetSharedResource("common","producer_complete");
                auto queue = provisioner->getSharedResource("common","queue");
                
                while(value<100){ //The goal is to pass 100 numbers in batches of 5
                    bool idle = true; // Flag to determine whether or not to put this thread to sleep.
                    {
                        ScopedLock l(queue->get()); //Lock the queue for safety. ScopedLock uses RAII

                        //std::cout<<"ScopedLock: "<<(l.isLocked()?"locked":"unlocked")<<std::endl;

                        auto vec = queue->safeGetPtrAs<std::vector<int>>(); //Get the pointer to the shared queue
                        
                        if(vec && vec.value()->size()<max_size){
                            //If the queue is valid and has less than 5 elements, append the next element
                            // and increment the value variable afterwards
                            vec.value()->emplace_back(value++);
                            std::cout<<"Producer Task: Enqueued "<<value-1<<std::endl;
                            std::cout<<"Producer Thread: Queue Size: "<<vec.value()->size()<<std::endl;
                            queue->get().unlock();
                            idle = false;
                        }
                        //If the queue is invalid, the idle flag stays true and triggers a sleep operation
                    }
                    if(idle)std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    idle = true;// Reset the idle flag for the next iteration.
                }

                //When all data has been transferred, safely set the completion flag
                // and return
                complete_flag->get().lock();
                auto producer_complete = complete_flag->safeGetPtrAs<bool>();
                if(producer_complete)**producer_complete=true;
                complete_flag->get().unlock();
                return 0;
            }
            ,nullptr
        )
    );

    consumer->addChainSegment(
        TaskNode::create<int>("consumer_task",
            [provisioner=app.getSharedProvisioner()]()->int{

                //Get the maximum size of the queue from the shared resource provisioner
                auto max_size = provisioner->getSharedResource("common","queue_limit")->safeGetAs<int>(true);
                
                //If the max size of the shared queue could not be obtained, use 5 as the default
                if(!max_size){
                    std::cerr<<"Consumer Task: Failed to get Max Size! Using 5...\n";
                    max_size = 5;
                }

                //Get the SharedResource pointers for the completion flag and the shared queue
                auto complete_flag = provisioner->safeGetSharedResource("common","producer_complete");
                auto queue = provisioner->getSharedResource("common","queue");

                //Loop control variable which will synchronize with the shared completion flag
                bool done = false;

                //Stringstream for easier printing of consumed values
                std::stringstream ss;

                while(!done){
                    //Start by trying to lock the shared queue but timeout after 100ms
                    if(queue->get().tryLock(100)){
                        //Safely get the shared queue pointer
                        auto vec = queue->safeGetPtrAs<std::vector<int>>();
                        //Safely get the completion flag
                        auto complete = complete_flag->safeGetAs<bool>(true);
                        if(complete && *complete){
                            //If the completion flag was set, synchronize with
                            // the local loop control variable
                            std::cout<<"Consumer Thread found Complete flag!\n";
                            done = true;
                        }

                        if(vec && (vec.value()->size()>=max_size || done)){
                            //If the shared queue is full, print out the contents
                            // and clear all elements in the queue.
                            ss.str("");
                            if(!vec.value()->empty()){ //Only print if the queue is not empty
                                ss<<"Consumer Thread: Found items: ";
                                for(auto& i : *vec.value())ss<<i<<" ";
                                ss<<"\n";
                                std::cout<<ss.str()<<std::endl;
                            }
                            vec.value()->clear();

                            //Finally, unlock the queue
                            queue->get().unlock();
                        }
                        else{
                            //Otherwise unlock the queue and sleep for 100ms
                            queue->get().unlock();
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        }
                    }                    
                }
                return 0;
            }
            ,nullptr
        )
    );

    //Create Workers
    app.addWorker("ProducerWorker"); //Add a worker thread for the producer
    app.addWorker("ConsumerWorker"); //Add a worker thread for the consumer

    //Assign chains to workers
    app.assignWorkerTask("ProducerWorker",producer); //Assign the producer task chain to the producer thread
    app.assignWorkerTask("ConsumerWorker",consumer); //Assign the consumer task chain to the consumer thread

    //Execute
    app.run(); //Run the workers
    
    /* In this case the quit function can be called immediately 
    * after the call to run because quit blocks until all 
    * worker threads have been joined and the tasks assigned to the workers
    * have their own internal loops. If the task chain were to be written such that
    * the chain itself formed the looping control flow, then the workers would need 
    * to be checked and synchronized manually before calling quit.
    */
    app.quit();

    return 0;
}