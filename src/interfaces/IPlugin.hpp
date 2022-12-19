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

class GenericFunction{
public:
    using Ptr = std::shared_ptr<GenericFunction>;
    GenericFunction() = default;

    void setFunc(std::function<std::optional<std::any>(std::initializer_list<std::any>)> fn){_func=fn;};

    template<typename T, typename... Args>
    std::optional<T> call(Args&&... args){
        if(!_func)return std::nullopt;
        auto type_list = {std::type_index(typeid(Args))...};

        std::optional<std::any> result;
        
        try{
            if(type_list.begin()->name() == typeid(AnyList).name() && type_list.size()==1){
                result = _func(std::any_cast<AnyList>(*AnyList{args...}.begin()));
            }
            else{
                result = _func(AnyList{args...});
            }
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

class IPlugin{
public:
    virtual ~IPlugin() = default;

protected:
    std::unordered_map<std::string,GenericFunction::Ptr> funcs;
};

#endif