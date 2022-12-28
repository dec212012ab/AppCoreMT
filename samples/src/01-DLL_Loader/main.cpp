#include <iostream>
#include <string>

#include "IPlugin.hpp"
#include "AppWorker.hpp"
#include "DynamicLibLoader.hpp"


int main()
{
    auto loader = std::make_shared<DynamicClass<GenericPlugin>>("./01-DLL.dll");

    GenericPlugin::Ptr greeter = loader->createSharedInstance();
    greeter->create();

    loader.reset();

    if(!greeter)throw std::runtime_error("Greeter 1 is null!");

    greeter->printAvailableFunctions();

    std::cout<<greeter->callFunction<std::string>("Greet").value_or("Unset!")<<std::endl;
    std::cout<<greeter->callFunction<std::string>("InvalidCall").value_or("Unset!")<<std::endl;
    return 0;
}
