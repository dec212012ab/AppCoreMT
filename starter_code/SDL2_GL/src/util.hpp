#pragma once

#include <iostream>
#include "Alias.hpp"


std::ostream& operator<<(std::ostream& output, AppCoreGui::Vec3f& vec)
{
    return output<<"Vec3("<<vec.x<<","<<vec.y<<","<<vec.z<<")";
}
