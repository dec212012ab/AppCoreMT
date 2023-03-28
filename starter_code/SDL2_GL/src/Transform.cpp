#include "Transform.hpp"

namespace AppCoreGui{

    Transform::Transform(Vec3f position, Vec3f rotation, Vec3f scale):
        position(position),rotation(rotation),scale(scale)
    {
        model_matrix = Mat4f(1.0f);
    }

}