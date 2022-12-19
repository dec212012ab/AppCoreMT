#ifndef __BRANCH_NODE_HPP__
#define __BRANCH_NODE_HPP__

#include <vector>

#include "TaskNode.hpp"

namespace AppCore{

class BranchNode : public virtual Node {
public:
    using Ptr = std::shared_ptr<BranchNode>;
    using UPtr = std::unique_ptr<BranchNode>;
    BranchNode();
    ~BranchNode() = default;

    static BranchNode::Ptr create(std::function<int(void)> selector_fn, std::vector<Node::Ptr> options) {
        BranchNode::Ptr b = std::make_shared<BranchNode>();
        b->setBranchSelector(selector_fn);
        b->setBranchOptions(options);
        return b;
    }

    void setBranchSelector(std::function<int(void)> fn) { branch_selector = fn; };
    void setBranchOptions(std::vector<Node::Ptr> opts) { branch_options.clear(); branch_options.swap(opts); };

    std::vector<Node::Ptr>& getBranchOptions() { return branch_options; };

    bool call() override;

    virtual Node::Ptr getNextNode() override;
    virtual Node::Ptr setNextNode(Node::Ptr n) override {
        next = n;
        return std::make_shared<BranchNode>(*this);
    };

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

};

#endif