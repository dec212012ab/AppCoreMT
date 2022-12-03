
#include <iostream>
#include "SafeVar.hpp"
#include "Version.hpp"

int main()
{
    std::cout<<"AppCoreMT Sample App Version "<<VERSION<<std::endl;
    SafeVar<int> sample_int = 0;
    return 0;
}