#ifndef __TASK_CHAIN_HPP__
#define __TASK_CHAIN_HPP__

#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "AppTask.hpp"


class Node {
public:
    using Ptr = std::shared_ptr<Node>;
    using UPtr = std::unique_ptr<Node>;
    enum class NodeType {
        Base,
        Task,
        Branch
    };
    Node() :node_type(NodeType::Base) {};

    NodeType getNodeType() { return node_type; };

    Node::Ptr setNextNode(Node::Ptr n) { next = n; return next; };
    virtual Node::Ptr getNextNode() { return next; };
    virtual bool call() { return false; };
protected:
    NodeType node_type;
    Node::Ptr next;
};

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

private:
    std::string task_name;
    TaskFn _fn;    
};

class BranchNode : public virtual Node {
public:
    using Ptr = std::shared_ptr<BranchNode>;
    using UPtr = std::unique_ptr<BranchNode>;
    BranchNode();
    ~BranchNode() = default;

    void setBranchSelector(std::function<int(void)> fn) { branch_selector = fn; };
    void setBranchOptions(std::vector<Node::Ptr> opts) { branch_options.clear(); branch_options.swap(opts); };

    std::vector<Node::Ptr>& getBranchOptions() { return branch_options; };

    bool call() override;

    virtual Node::Ptr getNextNode() override;

    void setRememberLastSelection(bool b) { remember_last_selection = b; };
    bool getRememberLastSelection() { return remember_last_selection; };

#ifdef TESTING
    std::function<int(void)>& getBranchSelectorFn() { return branch_selector; };
#endif

private:
    std::vector<Node::Ptr> branch_options;
    std::function<int(void)> branch_selector;
    bool remember_last_selection = true;
};

class TaskChain {
public:
    TaskChain() = default;



    
private:
    std::list<Node::Ptr> nodes;
};


#endif
