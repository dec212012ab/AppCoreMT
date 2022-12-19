#include "Node.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

using namespace AppCore;

Node::Ptr Node::setNextNode(Node::Ptr n)
{
	next = n;
	//if(n)n->parents.emplace_back(this);
	return std::make_shared<Node>(*this);
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