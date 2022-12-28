#ifndef __DLL_H__
#define __DLL_H__

#include "IPlugin.hpp"

class PluginGreet : public GenericPlugin{
public:
    PluginGreet():message("This is a greeting!"){};

    std::string getMessage(){return message;};
    void setMessage(std::string msg){message=msg;};

    virtual void create() override{
        std::cout<<"Creating Plugin_Greet"<<std::endl;        
        GenericFunction greet;
        greet.setFunc([this](AnyList args)->std::optional<std::any>{return this->getMessage();});
        addCallbackFunction("Greet",greet);
    }
protected:
    std::string message;
};

extern "C"{
    EXPORT GenericPlugin* create(){
        return new PluginGreet();
    };
    EXPORT void destroy(GenericPlugin* p){
        delete p;
    }
};

#endif