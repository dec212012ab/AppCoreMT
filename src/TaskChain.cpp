#include "TaskChain.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

using namespace AppCore;

void TaskChain::addChainSegment(Node::Ptr segment_root, uint8_t priority)
{
	if (segment_root) {
		if (chain_segments.find(priority) == chain_segments.end())
			chain_segments[priority] = std::list<Node::Ptr>();
		chain_segments[priority].emplace_back(segment_root);
	}
}

bool TaskChain::executeChain(bool allow_exceptions, int priority_filter)
{
	std::function<void(void)> _impl = [&]()->void {
		std::stringstream ss;
		if (priority_filter < 0) {
			for (const auto& seg : chain_segments) {
				int segment_index = 0;
				for (const auto& n : seg.second) {
					int node_index = 0;
					Node::Ptr iter = n;
					do {
						ss.str("");
						ss << "[Task Info]: Priority " << static_cast<int>(seg.first) << " Segment Index: " << segment_index;
						if (iter->getNodeType() == Node::NodeType::Base)ss << " Type: Node ";
						else if (iter->getNodeType() == Node::NodeType::Branch)ss << " Type: Branch ";
						else ss << " Type: Task [" << std::dynamic_pointer_cast<TaskNode>(iter)->getTaskID() << "] ";
						ss << "Index: " << node_index;
#ifdef TESTING
						std::cout << ss.str() << std::endl;
#endif
						if (!iter->call()) {							
							throw std::runtime_error("TaskChain Error: Node execution returned false!\n( " + ss.str() + ")");
						}
						iter = iter->getNextNode();
						node_index++;
					} while (iter);
					segment_index++;
				}
			}
		}
		else {
			int segment_index = 0;
			for (const auto& n : chain_segments[priority_filter]) {
				int node_index = 0;
				Node::Ptr iter = n;
				do {
					ss.str("");					
					ss << "[Task Info2]: Priority " << static_cast<int>(priority_filter) << " Segment Index: " << segment_index;
					if (iter->getNodeType() == Node::NodeType::Base)ss << " Type: Node ";
					else if (iter->getNodeType() == Node::NodeType::Branch)ss << "Type: Branch ";
					else ss << " Type: Task [" << std::dynamic_pointer_cast<TaskNode>(iter)->getTaskID() << "] ";
					ss << "Index: " << node_index;
#ifdef TESTING
					std::cout << ss.str() << std::endl;
#endif
					if (!iter->call()) {
						throw std::runtime_error("TaskChain Error: Node execution returned false!\n( "+ ss.str() + ")");
					}
					iter = iter->getNextNode();
					node_index++;
				} while (iter);
				segment_index++;
			}
		}
	};

	if (!allow_exceptions) {
		try {
			_impl();
		}
		catch (const std::exception& e) {
			std::cerr << "Caught exception when executing TaskChain: " << e.what() << std::endl;
			return false;
		}
		catch (...) {
			std::cerr << "Caught unknown error when executing TaskChain!" << std::endl;
			return false;
		}
	}
	else _impl();

	return true;
}

#ifdef TESTING //TaskChain
SCENARIO("TaskChain Build Test", "[TaskChain]"){
	GIVEN("An empty TaskChain instance") {
		TaskChain t;
		int r1 = 0;
		
		BranchNode::Ptr b = BranchNode::create(
			[&r1]()->int {
				std::cout << "Branch:" << r1 << std::endl;
				if (r1 > 8)return 2;
				else if (r1 >= 4)return 1;
				else return 0;
			}, 
			{}
		);
		b->setBranchOptions(
			{
				TaskNode::create<int, int>(AppTask<int, int>::create("Double", [](int a) {std::cout << "x2:" << 2 * a << std::endl; return 2 * a; }),&r1,std::move(r1))
					->setNextNode(b),
				TaskNode::create<int, int>(AppTask<int, int>::create("Triple", [](int a) {std::cout << "x3:" << 3 * a << std::endl; return 3 * a; }),&r1,std::move(r1))
					->setNextNode(b),
				TaskNode::create<int, int>(AppTask<int, int>::create("Quadruple", [](int a) {std::cout << "x4:" << 4 * a << std::endl; return 4 * a; }),&r1,std::move(r1))
			}
		);
		b->setRememberLastSelection(false);

		WHEN("Nodes are added") {			
			t.addChainSegment(
				TaskNode::create<int>(AppTask<int>::create("One", []()->int {return 1; }), &r1)
				->setNextNode(
					TaskNode::create<int, int>(
						AppTask<int, int>::create("Double", [](int a)->int {std::cout << "DOUBLE:" << 2 * a << std::endl; return 2 * a; }),
						&r1,
						std::move(r1)
					)
				->setNextNode(b)
				)
			);
			THEN("All nodes are contained and accessible at priority 100") {
				REQUIRE((t.getSegments().find(100) != t.getSegments().end()));
				REQUIRE(t.getSegments()[100].size() == 1);
			}
			AND_THEN("Executing the chain results with a final output of 48") {
				REQUIRE(t.executeChain(true));
				REQUIRE(r1 == 48);
			}
		}
	}
}
#endif
