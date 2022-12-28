
#include <iostream>
#include "SafeVar.hpp"
#include "TaskChain.hpp"
#include "Version.hpp"

using namespace AppCore;

int main()
{
    std::cout<<"AppCoreMT Sample App Version "<<VERSION<<std::endl;
    SafeVar<int> sample_int = 0;
    TaskChain chain;

    int sum = 0;

    chain.addChainSegment(
        TaskNode::create<int,int,int>("add",[](int a, int b)->int{return a+b;},&sum,1,2)
    );
    
    chain.executeChain(true);

    std::cout<<"1+2="<<sum<<std::endl;

    return 0;
}