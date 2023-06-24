#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <functional>
#include <memory>
#include <string>

namespace AppCore{

class Node {
public:
    using Ptr = std::shared_ptr<Node>;
    using UPtr = std::unique_ptr<Node>;
    enum class NodeType {
        Base=1,
        Task=2,
        Branch=3
    };
    Node() :node_type(NodeType::Base),next(nullptr) {};

    NodeType getNodeType() { return node_type; };

    virtual Node::Ptr setNextNode(Node::Ptr n); 
    virtual Node::Ptr getNextNode() { return next; };
    virtual bool call() { return false; };
    //bool isHeadNode() { return parents.empty(); };

protected:
    NodeType node_type;
    //std::list<Node*> parents;
    Node::Ptr next;
};

};

#endif