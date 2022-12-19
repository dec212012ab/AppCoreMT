#ifndef __TASK_CHAIN_HPP__
#define __TASK_CHAIN_HPP__

#include <cstdint>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "BranchNode.hpp"

namespace AppCore{

class TaskChain {
public:
    using Ptr = std::shared_ptr<TaskChain>;
    TaskChain() = default;
    void addChainSegment(Node::Ptr segment_root, uint8_t priority=100);
    bool executeChain(bool allow_exceptions, int priority_filter = -1);    
    std::map<uint8_t, std::list<Node::Ptr>>& getSegments() { return chain_segments; };
private:    
    std::map<uint8_t, std::list<Node::Ptr>> chain_segments;
};

}
#endif
