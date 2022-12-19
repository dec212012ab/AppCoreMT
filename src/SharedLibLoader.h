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

template <class T>
class IDynamicLibLoader{
public:
    using Ptr = std::shared_ptr<T>;
    virtual ~SharedLibLoader()=default;
    virtual void openSharedLib() = 0;
    virtual Ptr  getSharedLibInstance() = 0;
    virtual void closeSharedLib() = 0;
};

template<class T>
class DynamicLibraryLoader : public IDynamicLibLoader<T>{
public:
    DynamicLibraryLoader(const std::string&& path, const std::string&& alloc_symbol, const std::string&& delete_symbol )
        :_path_to_lib(path), _alloc_class_symbol(alloc_symbol), _delete_class_symbol(delete_symbol), handle(nullptr){};
    
    ~DynamicLibraryLoader()=default;

    void openSharedLib() override{
#if defined(_WIN32)
        if(!(_handle = LoadLibrary(_path_to_lib.c_str()))){
            throw std::runtime_error("openSharedLib: LoadLibrary error! [" + _path_to_lib + "]");
        }
#elif defined(__linux__)
        if(!(_handle = dlopen(_path_to_lib.c_str(),RTLD_NOW|RTLD_LAZY))){
            throw std::runtime_error("openSharedLib: dlopen error! [" + _path_to_lib + "] : " + dlerror());
        }
#endif
    };

    void closeSharedLib() override{
#if defined(_WIN32)
        if(FreeLibrary(_handle)==0){
            throw std::runtime_error("closeSharedLib: Failed to close library with FreeLibrary() ["+_path_to_lib+"]");
        }
#elif defined(__linux__)
        if(dlclose(_handle) != 0){
            throw std::runtime_error("closeSharedLib: Failed to close library with dlclose() [" + _path_to_lib+"] : " + dlerror());
        }
#endif
    };

    Ptr getSharedLibInstance() override{
        using allocClass = T*(*)();
        using deleteClass= void(*)(T*);
#ifdef _WIN32
    auto alloc_func = reinterpret_cast(GetProcAddress(_handle,_alloc_class_symbol.c_str()));
    auto delete_func = reinterpret_cast(GetProcAddress(_handle,_delete_class_symbol.c_str()));

    return std::shared_ptr(alloc_func(),[delete_func](T* p){delete_func(p);});
#elif defined(__linux__)
    auto alloc_func = reinterpret_cast(dlsym(_handle,_alloc_class_symbol.c_str()));
    auto delete_func= reinterpret_cast(dlsym(_handle,_delete_class_symbol.c_str()));
#endif
        if(!alloc_func || !delete_func){
            closeSharedLib();
            std::cerr<<"Allocator or deleter not found. Could not open library @ "<<_path_to_lib<<std::endl;
        }
    };

private:
#ifdef _WIN32
    HMODULE _handle;
#endif
#ifdef __linux__
    void* _handle;
#endif

    std::string _path_to_lib;
    std::string _alloc_class_symbol;
    std::string _delete_class_symbol;

};

}

#endif