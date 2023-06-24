#include "BranchNode.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

using namespace AppCore;

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