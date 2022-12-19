#include "TaskNode.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

using namespace AppCore;

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