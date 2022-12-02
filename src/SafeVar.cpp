#include "SafeVar.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

#ifdef TESTING
#include <vector>
//SafeVar
SCENARIO("SafeVar General Use","[SafeVar]"){
    GIVEN("A SafeVar Instance with primitive type"){
        SafeVar<int> var(0);
        THEN("It starts with value 0"){
            REQUIRE(var.get() == 0);
        }
        WHEN("The var is incremented with ++"){
            var.get()++;
            THEN("The var has value 1"){
                REQUIRE(var.get() == 1);
            }
        }
    }
    GIVEN("A SafeVar Instance with object type"){
        SafeVar<std::string> var("");
        THEN("It starts with an empty string"){
            REQUIRE(var.get() == "");
            REQUIRE(var.get().empty());
        }
        WHEN("A character is concatenated"){
            var.get() += "A";
            THEN("The string's length increases by one and holds the character"){
                REQUIRE(var.get().size()==1);
                REQUIRE(var.get() == "A");
            }
        }
    }
}
#endif