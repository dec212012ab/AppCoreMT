#ifndef __TASK_NODE_HPP__
#define __TASK_NODE_HPP__

#include "AppTask.hpp"
#include "Node.hpp"

namespace AppCore{

class TaskNode : public virtual Node{
public:
    using Ptr = std::shared_ptr<TaskNode>;
    using UPtr = std::unique_ptr<TaskNode>;
    using TaskFn = std::function<bool(void)>;
    
    TaskNode();

    template<typename R, typename... Args>
    static TaskNode::Ptr create(std::shared_ptr<AppTask<R, Args...>> t, R* output, Args&&... args) {
        TaskNode::Ptr task = std::make_shared<TaskNode>();
        task->setTask<R, Args...>(t, output, std::forward<Args>(args)...);
        return task;
    };

    template<typename R, typename... Args>
    static TaskNode::Ptr create(std::string task_id,std::function<R(Args...)> fn, R* output, Args&&... args){
        TaskNode::Ptr task = std::make_shared<TaskNode>();
        task->setTask(AppTask<R,Args...>::create(task_id,fn),output,std::forward<Args>(args)...);
        return task;
    }

    ~TaskNode() = default;
    operator TaskFn() { return _fn; };
    
    template<typename R, typename... Args>
    void setTask(std::shared_ptr<AppTask<R,Args...>> t,R* output, Args&&... args){        
        task_name = t->getTaskID();
        _fn = [t,output,&args...]()->bool {
            std::optional<R> result = t->run(std::forward<Args>(args)...);
            if (!result) {
                std::cerr << t->getTaskID()<<" : Optional is empty!" << std::endl;
                return false;
            }
            if (result && output) {
                *output = result.value();
            }
            return true;
        };        
    }

    bool call() override{if(!_fn)return false;return _fn();};
    std::string getTaskID() { return task_name; };
    void reset();

    Node::Ptr getNextNode()override { return next; };
    virtual Node::Ptr setNextNode(Node::Ptr n) override {
        next = n;
        return std::make_shared<TaskNode>(*this);
    };

private:
    std::string task_name;
    TaskFn _fn;    
};

};
#endif