#pragma once

#include "Alias.hpp"
#include "Component.hpp"
#include "Transform.hpp"

namespace AppCoreGui{

class TransformComponent : public virtual ComponentBase{
public:
    TransformComponent(Vec3f position=Vec3f(0,0,0), Vec3f rotation=Vec3f(0,0,0), Vec3f scale=Vec3f(0,0,0));
    
    //Global Space
    Vec3f getPosition();
    Vec3f getRotation();
    Vec3f getScale();
    void setPosition(Vec3f new_position);
    void setRotation(Vec3f new_rotation);
    void setScale(Vec3f new_scale);

    Vec3f translate(Vec3f offset);
    Vec3f rotate(Vec3f offset);
    Vec3f scale(Vec3f offset);

    //Local Space
    Vec3f getRelativePosition();
    Vec3f getRelativeRotation();
    Vec3f getRelativeScale();
    void setRelativePosition(Vec3f new_position);
    void setRelativeRotation(Vec3f new_position);
    void setRelativeScale(Vec3f new_position);
    
    Vec3f translateLocally(Vec3f offset);
    Vec3f rotateLocally(Vec3f offset);
    Vec3f scaleLocally(Vec3f offset);

    //Arbitrary Origins
    Vec3f translateRelativeTo(Vec3f offset, Vec3f origin);
    Vec3f rotateRelativeTo(Vec3f offset, Vec3f origin);
    Vec3f scaleRelativeTo(Vec3f offset, Vec3f origin);

    //Render Specific
    glm::mat4 getLocalTransformMatrix();

    
protected:
    Transform transform;

};

};