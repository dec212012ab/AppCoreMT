#include "TaskChain.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

Node::Ptr Node::setNextNode(Node::Ptr n)
{
	next = n;
	//if(n)n->parents.emplace_back(this);
	return std::make_shared<Node>(*this);
}

TaskNode::TaskNode()
{
	node_type = NodeType::Task;
	next = nullptr;
}

void TaskNode::reset()
{
	task_name = "";
	_fn = []()->bool {return false; };
}

BranchNode::BranchNode()
{
	node_type = NodeType::Branch;
	next = nullptr;
}

bool BranchNode::call()
{
	if (!branch_selector)return false;
	if (branch_options.empty())return false;
	int selected_branch = branch_selector();
	if (selected_branch >= 0 && selected_branch < branch_options.size()) {
		next = branch_options[selected_branch];
		return true;
	}
	return false;
}

Node::Ptr BranchNode::getNextNode()
{
	if (remember_last_selection)return next;
	Node::Ptr n = next;
	next = nullptr;
	return n;
}

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

#ifdef TESTING //Node
SCENARIO("Node Base Class Tests", "[Node]") {
	GIVEN("A Node instance") {
		Node::Ptr n = std::make_shared<Node>();
		THEN("Node type is 'Base'") {
			REQUIRE(n->getNodeType() == Node::NodeType::Base);
		}
		WHEN("The next node is requested before setting it") {
			THEN("The next node should be a nullptr") {
				REQUIRE(n->getNextNode() == nullptr);
			}
		}
		AND_WHEN("The next node is set before requesting it") {
			Node::Ptr n2 = std::make_shared<Node>();
			n->setNextNode(n2);
			THEN("The next node variable is a valid pointer to the node that was set as the next node") {
				REQUIRE(n->getNextNode() == n2);
			}
		}
		AND_WHEN("A node chain is constructed manually") {
			n->setNextNode(std::make_shared<Node>())->getNextNode()
				->setNextNode(std::make_shared<Node>())->getNextNode()
				->setNextNode(n);
			THEN("The setNextNode function can be chained in sequence") {
				int num_unique_nodes = 1;
				Node::Ptr n2 = n->getNextNode();
				while (true) {
					if (n2 != n) {
						num_unique_nodes++;
						n2 = n2->getNextNode();
						continue;
					}
					else break;
				}
				REQUIRE(num_unique_nodes == 3);
			}
		}
	}
}
#endif

#ifdef TESTING //Task Node
SCENARIO("TaskNode Constructor Tests", "[TaskNode][TaskNode::TaskNode]") {
	GIVEN("An empty TaskNode instance") {
		TaskNode::Ptr n = std::make_shared<TaskNode>();
		THEN("The task name and function are empty") {
			REQUIRE(n->getTaskID().empty());
			REQUIRE(!static_cast<TaskNode::TaskFn>(*n));
		}
	}
	GIVEN("A TaskNode initialized with a task") {
		AppTask<int, char> t;
		t.setTaskFunction("Double char", [](char b)->int {return b * 2; });
		int result = 0;
		AppTask<int, char>::Ptr tp = std::make_shared<AppTask<int,char>>(t);
		TaskNode::Ptr n = TaskNode::create<int,char>(tp, &result, 2);
		THEN("The task properties should be properly accessible") {
			REQUIRE(n->getTaskID() == t.getTaskID());
			REQUIRE(TaskNode::TaskFn(*n)());
			REQUIRE(result == 4);			
		}
	}
};

SCENARIO("TaskNode setTask Test", "[TaskNode][TaskNode::setTask]") {
	GIVEN("An AppTask instance") {
		AppTask<int, int> t;
		t.setTaskFunction("Double Int", [](int a)->int {return 2 * a; });
		std::cout << "T's name: " << t.getTaskID() << std::endl;
		WHEN("The AppTask is stored in a TaskNode") {
			TaskNode n;
			int result = 0;
			n.setTask<int,int>(std::make_shared<AppTask<int,int>>(t), &result, 2);
			THEN("Calling the task will store the doubled input value in the external variable") {
				bool out = n.call();
				REQUIRE(out);
				REQUIRE(result == 4);
			}
		}
	}
};

SCENARIO("TaskNode Function Calling Tests", "[TaskNode][TaskNode::call]") {
	GIVEN("A TaskNode with a doubling task assigned") {
		AppTask<int, char> t;
		t.setTaskFunction("Double char", [](char a)->int {return 2 * a; });
		TaskNode n;
		int result = 0;
		n.setTask<int,char>(std::make_shared<decltype(t)>(t), &result, 3);
		THEN("Calling the task with input 3 should return 6") {
			n.call();
			REQUIRE(result == 6);
		}
	}
}
#endif

#ifdef TESTING //BranchNode
SCENARIO("BranchNode Branch Selection Tests", "[BranchNode][BranchNode::call]") {
	GIVEN("An empty BranchNode that remembers the last selection made") {
		BranchNode n;
		WHEN("The node is called without setting anything") {
			THEN("The function should return false and the contents of the node should be empty") {
				REQUIRE(!n.call());
				REQUIRE(!n.getBranchSelectorFn());
				REQUIRE(n.getBranchOptions().empty());
				REQUIRE(n.getNextNode() == nullptr);
			}
		}
		AND_WHEN("The node is called after setting just the selector function") {
			n.setBranchSelector([]()->int {return 3; });
			THEN("The call should fail and the next node pointer should remain nullptr") {
				REQUIRE(!n.call());
				REQUIRE(n.getNextNode() == nullptr);
			}
		}
		AND_WHEN("The node is called after adding branches, but the selector returns an invalid index") {
			n.setBranchSelector([]()->int {return 3; });
			n.setBranchOptions({
				TaskNode::create<int>(AppTask<int>::create("Ret 2",[]()->int {return 2; }),nullptr),
				TaskNode::create<std::string,size_t>(AppTask<std::string,size_t>::create("size_t to string",[](size_t a)->std::string {return std::to_string(a); }),nullptr,25)
			});
			THEN("The call should fail and the next node pointer should be nullptr") {
				REQUIRE(!n.call());
				REQUIRE(n.getNextNode() == nullptr);
			}
		}
		AND_WHEN("The node is called after adding branches and a selector that will output a valid index") {
			int selection_index = 0;
			n.setBranchSelector([&selection_index]()->int {return selection_index++; });
			int result1 = 0;
			std::string result2 = "";
			n.setBranchOptions({
				TaskNode::create<int>(AppTask<int>::create("Ret 2",[]()->int {return 2; }),&result1),
				TaskNode::create<std::string,size_t>(AppTask<std::string,size_t>::create("size_t to string",[](size_t a)->std::string {return std::to_string(a); }),&result2,25)
			});
			THEN("The call should succeed and return the correct value based on which branch is taken") {
				REQUIRE(n.call());
				n.getNextNode()->call();
				REQUIRE(result1 == 2);
				REQUIRE(selection_index == 1);
				REQUIRE(n.call());
				n.getNextNode()->call();
				REQUIRE(result2 == "25");
				REQUIRE(selection_index == 2);
				REQUIRE(!n.call());
			}
		}

	}
}
#endif

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
			Node::Ptr head = TaskNode::create<int>(AppTask<int>::create("One", []()->int {return 1; }), &r1);
			head->setNextNode(
				TaskNode::create<int, int>(
					AppTask<int, int>::create("Double", [](int a)->int {std::cout << "DOUBLE:" << 2 * a << std::endl; return 2 * a; }),
					&r1,
					std::move(r1)
				)
			)->getNextNode()->setNextNode(b);
			
			t.addChainSegment(
				head
			);
			THEN("All nodes are contained and accessible at priority 100") {
				REQUIRE((t.getSegments().find(100) != t.getSegments().end()));
				REQUIRE(t.getSegments()[100].size() == 1);
			}
			AND_THEN("Executing the chain results with a final output of 24") {
				REQUIRE(t.executeChain(true));
				REQUIRE(r1 == 48);
			}
		}
	}
}
#endif
