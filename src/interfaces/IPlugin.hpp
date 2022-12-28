#ifndef __IPLUGIN_HPP___
#define __IPLUGIN_HPP__

#ifdef _WIN32
    #ifndef APPCORE_EXPORT
        #define APPCORE_EXPORT __declspec(dllexport)
    #else
        #define APPCORE_EXPORT __declspec(dllimport)
    #endif
#else
    #define APPCORE_EXPORT
#endif

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

#include <any>
#include <initializer_list>
#include <optional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "TaskNode.hpp"

using namespace AppCore;

using AnyList=std::initializer_list<std::any>;

class APPCORE_EXPORT GenericFunction{
public:
    using Ptr = std::shared_ptr<GenericFunction>;
    GenericFunction() = default;

    void setFunc(std::function<std::optional<std::any>(std::initializer_list<std::any>)> fn){_func=fn;};

    template<typename T, typename... Args>
    std::optional<T> call(Args&&... args){
        if(!_func)return std::nullopt;
        /*auto type_list = {std::type_index(typeid(Args))...};

        std::cout<<"TYPE: "<<type_list.begin()->name()<<" vs "<<typeid(AnyList).name()<<std::endl;
        std::cout<<"SIZE: "<<type_list.size()<<std::endl;*/

        std::optional<std::any> result;
        
        try{
            result = _func(AnyList{args...});
        }
        catch(const std::bad_any_cast& e){
            std::cerr<<"Failed to cast function result to type "<<typeid(T).name()<<" !\n"<<e.what()<<std::endl;
            return std::nullopt;
        }

        if(!result)return std::nullopt;

        try{
            return std::any_cast<T>(result.value());
        }
        catch(const std::bad_any_cast& e){
            std::cerr<<"Failed to cast return value to type "<<typeid(T).name()<<" !\n"<<e.what()<<std::endl;
            return std::nullopt;
        }
    };

protected:
    std::function<std::optional<std::any>(std::initializer_list<std::any>)> _func;
};

class APPCORE_EXPORT IPlugin{
public:
    IPlugin():funcs(std::unordered_map<std::string,GenericFunction::Ptr>()){};
    virtual ~IPlugin() = default;

    virtual void create()=0;
    virtual void addCallbackFunction(std::string name, GenericFunction::Ptr callback)=0;
    virtual void addCallbackFunction(std::string name, GenericFunction& callback)=0;
    virtual std::vector<std::string> getAvailableFunctions()=0;
    virtual void printAvailableFunctions()=0;
    virtual std::unordered_map<std::string,GenericFunction::Ptr>& getFunctions(){
        return funcs;
    };

    template<typename T, typename... Args>
    std::optional<T> callFunction(std::string name, Args&&... args){
        if(funcs.find(name)!=funcs.end())return funcs[name]->call<T>(args...);
        return std::nullopt;
    }
protected:
    std::unordered_map<std::string,GenericFunction::Ptr> funcs;
};

class APPCORE_EXPORT GenericPlugin : public IPlugin{
public:
    using Ptr = std::shared_ptr<GenericPlugin>;
    virtual ~GenericPlugin()=default;

    virtual void create() override{};

    virtual void addCallbackFunction(std::string name, GenericFunction::Ptr callback) override{
        if(!callback)return;
        if(funcs.find(name)!=funcs.end()){
            std::cerr<<"The function name "<<name<<" has already been added! Cannot add the callback."<<std::endl;
            return;
        }
        funcs[name] = callback;
    };

    virtual void addCallbackFunction(std::string name, GenericFunction& callback) override{
        GenericFunction::Ptr g = std::make_shared<GenericFunction>(callback);
        addCallbackFunction(name,g);
    };

    virtual std::vector<std::string> getAvailableFunctions() override{
        std::vector<std::string> keys;
        int i = 0;
        for(auto iter = funcs.begin(); iter!=funcs.end(); iter++){
            std::cout<<"i: "<<i++<<std::endl;
            keys.emplace_back(iter->first);
        }
        return keys;
    };

    virtual void printAvailableFunctions() override{
        std::cout<<"Funcs size: "<<funcs.size()<<std::endl;
        for(auto iter = funcs.begin(); iter!=funcs.end(); iter++)
            std::cout<<"\t"<<iter->first<<std::endl;
    };

    typedef GenericPlugin* createObj();
    typedef void destroyObj(GenericPlugin*);

protected:
    
};

#endif