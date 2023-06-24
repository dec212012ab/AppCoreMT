#ifndef __BRANCH_NODE_HPP__
#define __BRANCH_NODE_HPP__

#include <vector>

#include "TaskNode.hpp"

namespace AppCore{

class BranchNode : public virtual Node {
public:
    using Ptr = std::shared_ptr<BranchNode>;
    using UPtr = std::unique_ptr<BranchNode>;

    /**
     * @brief Construct a new Branch Node object. Branch nodes
     * allow for conditional execution based on user-defined conditions
     * that may derive from outputs of previous tasks.
     * @warning Use the static create() function to create Branch nodes.
     * 
     */
    BranchNode();
    
    /**
     * @brief Destroy the Branch Node object
     * 
     */
    ~BranchNode() = default;

    /**
     * @brief Branch node creation function. 
     * 
     * @param selector_fn : Function which chooses which execution path to take next
     * @param options : Vector of nodes that may be selected for execution.
     * @return BranchNode::Ptr 
     */
    static BranchNode::Ptr create(std::function<int(void)> selector_fn, std::vector<Node::Ptr> options) {
        BranchNode::Ptr b = std::make_shared<BranchNode>();
        b->setBranchSelector(selector_fn);
        b->setBranchOptions(options);
        return b;
    }

    /**
     * @brief Set the selection function used to select the next task
     * 
     * @param fn : The selection function
     */
    void setBranchSelector(std::function<int(void)> fn) { branch_selector = fn; };

    /**
     * @brief Set the possible branch options
     * 
     * @param opts : Vector of Node pointers
     */
    void setBranchOptions(std::vector<Node::Ptr> opts) { branch_options.clear(); branch_options.swap(opts); };

    /**
     * @brief Get the vector of node options
     * 
     * @return std::vector<Node::Ptr>& 
     */
    std::vector<Node::Ptr>& getBranchOptions() { return branch_options; };

    /**
     * @brief Executes this node
     * 
     * @return true if the node was initialized and the call executed successfully
     * @return false if the node was not initialized or could not execute successfully
     */
    bool call() override;

    /**
     * @brief Get the Next Node object in the chain
     * 
     * @return Node::Ptr 
     */
    virtual Node::Ptr getNextNode() override;

    /**
     * @brief Set the Next Node object in the chain
     * 
     * @param n : Pointer to the next node object
     * @return Node::Ptr 
     */
    virtual Node::Ptr setNextNode(Node::Ptr n) override {
        next = n;
        return std::make_shared<BranchNode>(*this);
    };

    /**
     * @brief Set the Remember Last Selection flag. Subsequent calls to the
     * branch node will not call the selection function.
     * 
     * @param b : New state of the flag
     */
    void setRememberLastSelection(bool b) { remember_last_selection = b; };

    /**
     * @brief Get the Remember Last Selection flag state
     * 
     * @return true if the flag was set
     * @return false if the flag was not set
     */
    bool getRememberLastSelection() { return remember_last_selection; };

#ifdef TESTING
    std::function<int(void)>& getBranchSelectorFn() { return branch_selector; };
#endif

private:
    std::vector<Node::Ptr> branch_options;
    std::function<int(void)> branch_selector;
    bool remember_last_selection = true;
};

};

#endif