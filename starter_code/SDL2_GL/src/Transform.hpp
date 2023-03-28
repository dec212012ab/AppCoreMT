#pragma once

#include <glm/glm.hpp>
#include "Alias.hpp"

namespace AppCoreGui{

struct Transform{

    Transform(Vec3f position=Vec3f(0,0,0), Vec3f rotation=Vec3f(0,0,0), Vec3f scale=Vec3f(0,0,0));

    //Local Space Information
    Vec3f position;
    Vec3f rotation;//Currently euler. Should transition to quaternion with euler representation on the user side
    Vec3f scale;

    //Global Space Information
    Mat4f model_matrix;
};

}