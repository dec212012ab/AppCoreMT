#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Alias.hpp"

namespace AppCoreGui{

class Transform{
public:
    Transform(Vec3f position=Vec3f(0,0,0), Vec3f rotation=Vec3f(0,0,0), Vec3f scale=Vec3f(1,1,1));

    Vec3f getPosition();
    Vec3f getRotation();
    Vec3f getScale();
    void setPosition(Vec3f new_position);
    void setRotation(Vec3f new_rotation);
    void setScale(Vec3f new_scale);

    Vec3f translate(Vec3f offset);
    Vec3f translate(float x, float y, float z);
    Vec3f translateX(float x);
    Vec3f translateY(float y);
    Vec3f translateZ(float z);

    Vec3f rotate(Vec3f offset);
    Vec3f rotate(float x, float y, float z);
    Vec3f rotateX(float x);
    Vec3f rotateY(float y);
    Vec3f rotateZ(float z);
    
    Vec3f scale(Vec3f offset);
    Vec3f scale(float x, float y, float z);
    Vec3f scaleX(float x);
    Vec3f scaleY(float y);
    Vec3f scaleZ(float z);

    operator Mat4f();

protected:
    //Local Space Information
    Vec3f position;
    Vec3f rotation;//Currently euler. Should transition to quaternion with euler representation on the user side
    Vec3f _scale;
};

}