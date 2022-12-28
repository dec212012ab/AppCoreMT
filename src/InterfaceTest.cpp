#include "IPlugin.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

using namespace AppCore;


#ifdef TESTING //GenericFunction

SCENARIO("Generic Function Test","[GenericFunction]"){
    GIVEN("A generic function"){
        GenericFunction g;
        g.setFunc([](std::initializer_list<std::any> args)->std::optional<std::any>{
            //size_t argsize = sizeof...(args);
            //if(argsize!=3)return std::nullopt;
            std::vector<std::any> arglist = args;
            if(arglist.size()!=3)return std::nullopt;
            std::string output = "";
            for(int i = 0; i<arglist.size(); i++){
                std::string type_name = "unset";
                try{
                    switch(i){
                        case 0:{
                            type_name = typeid(std::string).name();
                            output += std::any_cast<std::string>(arglist[i]);
                            output += " , ";
                            break;
                        }
                        case 1:{
                            type_name = typeid(int).name();
                            output += std::to_string(std::any_cast<int>(arglist[i]));
                            output+= " , ";
                            break;
                        }
                        case 2:{
                            type_name = typeid(char).name();
                            output += std::any_cast<char>(arglist[i]);
                            break;
                        }
                        default:{
                            std::cerr<<"Reached default cast branch for argument with index "<<i<<std::endl;
                            return std::nullopt;
                        }
                    }
                }
                catch(const std::exception& e){
                    std::cerr<<"Arg failed to be casted to \""<<type_name<<"\". Actual type is: "<<arglist[i].type().name()<<std::endl;
                    return std::nullopt;
                }
            }
            return output;
        });
        THEN("The function should throw for improper types"){
            REQUIRE_FALSE(g.call<std::string>(1,23,4));
            REQUIRE_FALSE(g.call<std::string>(AnyList{5}));
        }
        THEN("The function should not throw for proper types"){
            std::optional<std::string> result = g.call<std::string>(std::string("The number is "),5,'a');
            REQUIRE(result);
            
            result = g.call<std::string>(AnyList{std::string("The number is not"),6,'b'});
            REQUIRE(result);
        }
    }
};

#endif

#ifdef TESTING

SCENARIO("Plugin Test","[GenericPlugin]"){
    GIVEN("An Plugin instance"){
        GenericPlugin p;
        WHEN("A GenericFunction is added and called"){
            GenericFunction g;
            g.setFunc([](std::initializer_list<std::any> args)->std::optional<std::any>{
                std::vector<std::any> arglist = args;
                if(arglist.size()!=3)return std::nullopt;
                std::string output = "";
                for(int i = 0; i<arglist.size(); i++){
                    std::string type_name = "unset";
                    try{
                        switch(i){
                            case 0:{
                                type_name = typeid(std::string).name();
                                output += std::any_cast<std::string>(arglist[i]);
                                output += " , ";
                                break;
                            }
                            case 1:{
                                type_name = typeid(int).name();
                                output += std::to_string(std::any_cast<int>(arglist[i]));
                                output+= " , ";
                                break;
                            }
                            case 2:{
                                type_name = typeid(char).name();
                                output += std::any_cast<char>(arglist[i]);
                                break;
                            }
                            default:{
                                std::cerr<<"Reached default cast branch for argument with index "<<i<<std::endl;
                                return std::nullopt;
                            }
                        }
                    }
                    catch(const std::exception& e){
                        std::cerr<<"Arg failed to be casted to \""<<type_name<<"\". Actual type is: "<<arglist[i].type().name()<<std::endl;
                        return std::nullopt;
                    }
                }
                return output;
            });
            p.addCallbackFunction("test_fn",g);

            THEN("The Function returns the expected answer"){
                std::optional<std::string> result = p.callFunction<std::string>("test_fn",std::string("The number is "),5,'a');
                //REQUIRE(p.getFunctions()->find("test_fn")!=p.getFunctions()->end());
                //std::optional<std::string> result = p.getFunctions()->at("test_fn")->call<std::string>(std::string("The number is "),5,'a');
                REQUIRE(result);
                std::cout<<result.value()<<std::endl;
            }
        }
    }
};

#endif