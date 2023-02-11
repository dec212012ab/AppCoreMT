#ifndef __SHARED_LIB_LOADER_H__
#define __SHARED_LIB_LOADER_H__

#include <iostream>
#include <memory>
#include <string>

#ifdef __linux__
#include <dlfcn.h>
#endif

#ifdef _WIN32
#include "Windows.h"
#endif

namespace AppCore{

template<class T>
class DynamicClass{
public:
    using Ptr = std::shared_ptr<DynamicClass<T>>;
    DynamicClass(std::string module_name, std::string allocator_sym = "create", std::string destructor_sym = "destroy")
        :module_name(module_name), allocator_sym(allocator_sym), destructor_sym(destructor_sym)
    {
        shared_obj = std::make_shared<SharedObject>();
    };
    ~DynamicClass()=default;

    template<typename... Args>
    std::shared_ptr<T> createSharedInstance(Args... args){
        if(!shared_obj->isInitialized()){
            if(!shared_obj->openModule(module_name))return nullptr;
        }
        SharedObject::Ptr _shared = shared_obj;
        return std::shared_ptr<T>(shared_obj->create(args...),[_shared](T* p){_shared->destroy(p);});
    };

private:
    struct SharedObject{
        using Ptr = std::shared_ptr<SharedObject>;
        typename T::createObj* create = NULL;
        typename T::destroyObj* destroy = NULL;
#ifdef _WIN32
        HMODULE _handle = NULL;
#elif defined(__linux__)
        void* _handle = NULL;
#endif
        ~SharedObject(){closeModule();};
        
        bool openModule(std::string module_name, std::string allocator_sym = "create", std::string destructor_sym = "destroy"){
#if defined(_WIN32)
            if(!(_handle = LoadLibrary(module_name.c_str()))){
                throw std::runtime_error("openSharedLib: LoadLibrary error! [" + module_name + "]");
            }
            create = reinterpret_cast<typename T::createObj*>(GetProcAddress(_handle,allocator_sym.c_str()));
            destroy = reinterpret_cast<typename T::destroyObj*>(GetProcAddress(_handle,destructor_sym.c_str()));
#elif defined(__linux__)
            if(!(_handle = dlopen(_path_to_lib.c_str(),RTLD_NOW|RTLD_LAZY))){
                throw std::runtime_error("openSharedLib: dlopen error! [" + module_name + "] : " + dlerror());
            }
            create = reinterpret_cast<typename T::createObj*>(dlsym(_handle,allocator_sym.c_str()))
            destroy = reinterpret_cast<typename T::destroyObj*>(dlsym(_handle,destructor_sym.c_str()));
#endif
            if(!create || !destroy){
                closeModule();
                std::cerr<<"Allocator or destructor symbol not found. Could not open library @ "<<module_name<<std::endl;
                return false;
            }
            _module_name = module_name;
            return true;

        };
        
        void closeModule(){
            if(_handle){
#ifdef _WIN32
                if(FreeLibrary(_handle)==0){
                    throw std::runtime_error("closeSharedLib: Failed to close library with FreeLibrary() ["+_module_name+"]");
                }
#elif defined(__linux__)
                if(dlclose(_handle) != 0){
                    throw std::runtime_error("closeSharedLib: Failed to close library with dlclose() [" + _module_name +"] : " + dlerror());
                }
#endif
                _handle = NULL;
            }
            if(create)create=NULL;
            if(destroy)destroy=NULL;
            _module_name = "";
        };

        bool isInitialized(){return create && destroy;};
    private:
        std::string _module_name;
    };

    std::string module_name;
    std::string allocator_sym;
    std::string destructor_sym;
    std::shared_ptr<SharedObject> shared_obj;
};

}

#endif