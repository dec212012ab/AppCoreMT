#include "AppTask.hpp"

using namespace AppCore;

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif 

#ifdef TESTING
#include <sstream>
SCENARIO("Task Function Binding","[AppTask]"){
    GIVEN("A sample task that returns 1"){
        AppTask<int> t;
        t.setTaskFunction("AppTask Test Fn 1",[]()->int{return 1;});
        t.setAllowExceptions(true);
        THEN("Calling the function returns 1"){
            REQUIRE(t.run().value_or(0) == 1);
        }
    }
    GIVEN("A sample task that takes one input type") {
        AppTask<std::string, int> t;
        t.setTaskFunction("AppTask Test Fn 2",[](int a)->std::string {
            std::stringstream ss;
            ss << a;
            return ss.str();
        });
        t.setAllowExceptions(true);
        THEN("The function correctly executes and uses the input argument") {
            std::optional<std::string> result = t.run(5);
            REQUIRE(result.has_value());
            REQUIRE(result.value() == "5");
        }
    }
    GIVEN("A sample task that takes multiple input types") {
        AppTask<std::string,int,char,std::vector<size_t>> t;
        t.setTaskFunction("AppTask Test Fn 3",[](int a, char b, std::vector<size_t> c)->std::string{
            std::stringstream ss;
            ss<<"("<<typeid(a).name()<<") "<<a<<"\n";
            ss<<"("<<typeid(b).name()<<") "<<b<<"\n";
            ss<<"("<<typeid(c).name()<<")\n";
            for(const auto& i : c){
                ss<<"\t("<<typeid(i).name()<<") "<<i<<"\n";
            }
            return ss.str();
        });
        t.setAllowExceptions(true);
        THEN("The result properly contains the types and values of the input"){
            std::optional<std::string> result = t.run(24,'+',{1,2,3,4});
            REQUIRE(result.has_value());
            std::cout<<result.value_or("ERROR RESULT!")<<std::endl;
        }
    }
}

SCENARIO("Task Function Reference Modification","[AppTask]"){
    GIVEN("A sample task that tries to modify a value by reference"){
        AppTask<int,int&> t;
        t.setTaskFunction("AppTask Test Fn 4",[](int& a) {
            a++;
            return a;
        });
        int n = 0;
        t.setAllowExceptions(true);
        THEN("The original value changes when the task is run") {
            t.run(n);
            REQUIRE(n == 1);
        }
    }
}

SCENARIO("Task Function Exception Tests", "[AppTask]") {
    GIVEN("A task that throws an exception") {
        AppTask<int> t;
        t.setTaskFunction("AppTask Test Fn 5", []()->int {
            throw std::runtime_error("Test Fn 5 Threw an Exception!");
            return 1;
        });
        WHEN("Exceptions are allowed") {
            t.setAllowExceptions(true);
            THEN("An exception is thrown") {
                REQUIRE_THROWS(t.run());
            }
        }
        AND_WHEN("Exceptions are disallowed") {
            t.setAllowExceptions(false);
            THEN("The exception is caught and the result is invalid") {
                REQUIRE_NOTHROW(t.run());
                std::optional<int> result = t.run();
                REQUIRE_FALSE(result.has_value());
            }
        }
    }
}

SCENARIO("Task Function Retry Tests", "[AppTask]") {
    GIVEN("A task which throws an exception and is allowed to retry") {
        AppTask<int, int&> t;
        t.setTaskFunction("AppTask Test Fn 6",[](int& a)->int {
            std::stringstream ss;
            ss << "Retry trigger: " << a++;
            if(a<=3)throw std::runtime_error(ss.str());
            return 123;
        });
        t.setAllowExceptions(false);
        t.setAllowRetry(true);
        WHEN("Retry max count is set to 3") {
            t.setMaxRetries(3);
            THEN("Task throws 3 times before returning an invalid value") {
                int a = 0;
                std::optional<int> result = t.run(a);
                REQUIRE(t.getRetryCounter() == t.getMaxRetries());
                REQUIRE_FALSE(result.has_value());
            }
        }
        WHEN("Retry max count is set to 4") {
            t.setMaxRetries(4);
            THEN("Task throws 3 times before returning the value 123") {
                int a = 0;
                std::optional<int> result = t.run(a);
                REQUIRE(t.getRetryCounter() == 3);
                REQUIRE(result.has_value());
                REQUIRE(result.value() == 123);
            }
        }
    }
}
#endif


