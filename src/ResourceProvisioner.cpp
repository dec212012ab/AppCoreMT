#include "ResourceProvisioner.hpp"

using namespace AppCore;

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

#ifdef TESTING
#include <vector>
SCENARIO("SharedResource Tests","[SharedResource]"){
    GIVEN("An empty SharedResource Instance"){
        SharedResource res;
        THEN("The owning thread ID and variable name are empty and the underlying variable is of int type holding 0"){
            REQUIRE(res.getOwningThreadID().empty());
            REQUIRE(res.getVariableName().empty());
            REQUIRE(res.getAs<int>() == 0);
        }
    }

    GIVEN("A SharedResource Instance storing a string literal"){
        SharedResource res("owner","name","value to be promoted to string");
        THEN("The const char* is promoted to std::string and the specialized constructor is used"){
            REQUIRE(res.get().get().type().name() == typeid(std::string).name());
        }
    }

    GIVEN("A SharedResource Instance storing an arbitrary type"){
        struct ArbitraryType{
            int a = 5;
            std::vector<std::string> b;
            float c = 3.14159;
            ArbitraryType() = default;
            ArbitraryType(int i, std::vector<std::string> j, float k)
                :a(i),b(j), c(k){};
        };
        SharedResource res("owner","name",ArbitraryType(8,std::vector<std::string>{"a","b","c"},5280.0));
        THEN("The template constructor is used and the resource contents are correct"){
            REQUIRE(res.get().get().type().name() == typeid(ArbitraryType).name());
            REQUIRE_NOTHROW(res.getAs<ArbitraryType>());
            ArbitraryType a = res.getAs<ArbitraryType>();
            REQUIRE(a.a == 8);
            REQUIRE(a.b.size() == 3);
            REQUIRE(a.b[0] == "a");
            REQUIRE(a.b[1] == "b");
            REQUIRE(a.b[2] == "c");
            REQUIRE(typeid(decltype(a.c)).name() == typeid(float).name());
            REQUIRE(a.c == 5280.0);
        }
    }
};

#endif