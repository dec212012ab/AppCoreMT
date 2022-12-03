#ifndef __APP_CORE_HPP__
#define __APP_CORE_HPP__

#include <string>
#include <deque>
#include <unordered_map>
#include <thread>

#include "Version.hpp"

class AppCoreMT{
public:
    AppCoreMT();
    ~AppCoreMT();

    bool addWorker();
    bool endWorker();
    bool detachWorker();
    
    

private:

};

#endif