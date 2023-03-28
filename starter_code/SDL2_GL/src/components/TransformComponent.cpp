#include "TransformComponent.hpp"

namespace AppCoreGui{

    TransformComponent::TransformComponent(Vec3f position, Vec3f rotation, Vec3f scale):
        transform(position,rotation,scale)
    {
        component_type_id = AppCoreGui::getComponentTypeID<TransformComponent>();
        REGISTER_TYPENAME(getComponentTypeID(), "TransformComponent");
    }

    Vec3f TransformComponent::getPosition()
    {
        return transform.position;
    }

    Vec3f TransformComponent::getRotation()
    {
        return transform.rotation;
    }

    Vec3f TransformComponent::getScale()
    {
        return transform.scale;
    }

    void TransformComponent::setPosition(Vec3f new_position)
    {
        transform.position = new_position;
    }

    void TransformComponent::setRotation(Vec3f new_rotation)
    {
        transform.rotation = new_rotation;
    }

    void TransformComponent::setScale(Vec3f new_scale)
    {
        transform.scale = new_scale;
    }

    Vec3f TransformComponent::translate(Vec3f offset)
    {
        transform.position += offset;
        return transform.position;
    }



    

};