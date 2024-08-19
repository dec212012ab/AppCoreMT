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

/**
 * @brief Primary app container which is responsible for
 * dispatching tasks to worker threads.
 * 
 */
class AppCoreMT{
public:
    /**
     * @brief This struct encapsulates worker objects
     * and allows workers to be configured with a priority
     * as well as exception behavior.
     * 
     */
    struct WorkerObject{
        using Ptr = std::shared_ptr<WorkerObject>;
        AppWorker::Ptr worker;///< Pointer to the worker object
        bool allow_task_exceptions=true;///< If true, exceptions that are not explicitly handled will bubble up and possibly crash the program
        int task_priority_filter=-1; ///< Priority value filter limit.
    };

    /**
     * @brief Construct a new AppCoreMT object
     * 
     * @param max_worker_count: The maximum number of thread workers to allow.
     */
    AppCoreMT(size_t max_worker_count=0):max_worker_limit(max_worker_count){
        shared_resources = ResourceProvisioner::create();
    };
    ~AppCoreMT()=default;

    /**
     * @brief Function which runs the app, starts up workers and 
     * assigns tasks from the task queue as available.
     * 
     */
    virtual void run();

    /**
     * @brief Signals that it's time for the app to quit. Running workers
     * are stopped with blocking calls to join() before the rest of the app
     * is allowed to exit.
     * 
     */
    virtual void quit();

    /**
     * @brief Adds a new worker to the worker pool with the default configuration.
     * 
     * @return true if the worker was added to the pool
     * @return false if the worker limit has been reached
     */
    bool addWorker();

    /**
     * @brief Adds a new worker to the worker pool with the provided configuration.
     * 
     * @param name : The name of the worker
     * @param immortal: If true the worker cannot be stopped dynamically.
     * @param allow_task_exceptions : If false, exceptions triggered by a running task
     * will be handled internally instead of crashing the worker or possibly the program.
     * @param task_priority_filter : [WIP] Sets the task priority filter value for the worker.
     * @return true if the worker was added to the pool
     * @return false if the worker limit has been reached
     */
    bool addWorker(std::string name,bool immortal=false,bool allow_task_exceptions=true,int task_priority_filter=-1);

    /**
     * @brief Stops the worker found with at the specified index in the worker pool.
     * 
     * @param index : Index of the worker to stop
     * @return true if the worker has been stopped successfully
     * @return false if the index is invalid
     */
    bool endWorker(int index);

    /**
     * @brief Stops the worker with the specified name in the worker pool.
     * 
     * @param name : Name of the worker to stop in the pool (if found)
     * @return true if the worker has been stopped successfully
     * @return false if a worker with the provided name could not be found in the pool or if the worker is immortal
     */
    bool endWorker(std::string name);

    /**
     * @brief Assigns a task from the task queue to the worker found at the 
     * specified index in the worker pool.
     * 
     * @param index : Index of the worker to assign the task to.
     * @param task : Task being assigned to the worker
     * @return true if the task was successfully assigned to the worker
     * @return false if the index or worker at the index were invalid or uninitialized
     */
    bool assignWorkerTask(size_t index,TaskChain::Ptr task);

    /**
     * @brief Assigns a task from the task queue to the worker found with
     * the specified name in the worker pool.
     * 
     * @param name : Name of the worker which will be assigned the task
     * @param task : Task being assigned to the worker
     * @return true if the task was successfully assigned to the worker
     * @return false if the name or worker with the name were invalid or uninitialized
     */
    bool assignWorkerTask(std::string name,TaskChain::Ptr task);
    //bool detachWorker();
    
    /**
     * @brief Set the Max Worker Limit
     * 
     * @param count : Max number of worker threads allowed to be spawned.
     */
    void setMaxWorkerLimit(size_t count);

    /**
     * @brief Iterator to the front of the worker pool
     * 
     * @return std::deque<WorkerObject::Ptr>::iterator 
     */
    std::deque<WorkerObject::Ptr>::iterator begin(){return workers.begin();};

    /**
     * @brief Iterator to the end of the worker pool
     * 
     * @return std::deque<WorkerObject::Ptr>::iterator 
     */
    std::deque<WorkerObject::Ptr>::iterator end(){return workers.end();};
    
    /**
     * @brief Returns the number of workers in the worker pool
     * 
     * @return size_t 
     */
    size_t size(){return workers.size();};

    /**
     * @brief Retrieves a pointer to the worker object found
     * at the specified index.
     * 
     * @param index : Index of the worker to retrieve
     * @return WorkerObject::Ptr 
     */
    WorkerObject::Ptr at(size_t index);

    /**
     * @brief Retrieves a pointer to the worker object found
     * in the pool with the specified name.
     * 
     * @param name : Name of the worker to be retrieved
     * @return WorkerObject::Ptr 
     */
    WorkerObject::Ptr at(std::string name);

    /**
     * @brief Get a pointer to the Shared Resource Provisioner object
     * 
     * @return ResourceProvisioner::Ptr 
     */
    ResourceProvisioner::Ptr getSharedProvisioner(){return shared_resources;};

protected:
    std::deque<WorkerObject::Ptr> workers;///<Worker pool
    size_t max_worker_limit;///< Worker pool max limit
    ResourceProvisioner::Ptr shared_resources; ///< Pointer to App-global container providing access to shared resources.
};

}
#endif