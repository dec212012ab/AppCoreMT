#include <iostream>
#include <string>

#include "IPlugin.hpp"
#include "AppWorker.hpp"
#include "DynamicLibLoader.hpp"


int main()
{
    DynamicClass<GenericPlugin>::Ptr loader = std::make_shared<DynamicClass<GenericPlugin>>("./01-DLL.dll");
    GenericPlugin::Ptr greeter;    
    try{        
        greeter = loader->createSharedInstance();
    }
    catch(const std::exception& e){
        std::cerr<<"Caught an exception: "<<e.what()<<std::endl;
        return EXIT_FAILURE;
    }
    greeter->create();

    loader.reset();

    if(!greeter)throw std::runtime_error("Greeter 1 is null!");

    greeter->printAvailableFunctions();

    std::cout<<greeter->callFunction<std::string>("Greet").value_or("Unset!")<<std::endl;
    std::cout<<greeter->callFunction<std::string>("InvalidCall").value_or("Unset!")<<std::endl;
    return 0;
}
