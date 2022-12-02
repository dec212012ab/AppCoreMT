#include "TaskChain.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

TaskNode::TaskNode()
{
	node_type = NodeType::Task;

}

void TaskNode::reset()
{
	task_name = "";
	_fn = []()->bool {return false; };
}

BranchNode::BranchNode()
{
	node_type = NodeType::Branch;
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

void TaskChain::addBranchNode(std::initializer_list<Node::Ptr> branches) 
{
	BranchNode::Ptr b = std::make_shared<BranchNode>();
	b->setBranchOptions(branches);
	chain.emplace_back(b);
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
			n->setNextNode(std::make_shared<Node>())->setNextNode(std::make_shared<Node>())->setNextNode(n);
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
