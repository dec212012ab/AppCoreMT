#include "TransformComponent.hpp"
#include "Entity.hpp"

namespace AppCoreGui{

    TransformComponent::TransformComponent(Vec3f position, Vec3f rotation, Vec3f scale):
        transform(position,rotation,scale)
    {
        component_type_id = AppCoreGui::getComponentTypeID<TransformComponent>();
        REGISTER_TYPENAME(getComponentTypeID(), "TransformComponent");
        model_matrix = Mat4f();
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

    Vec3f TransformComponent::rotate(Vec3f offset)
    {
        transform.rotation += offset;
        return transform.rotation;
    }
    
    Vec3f TransformComponent::scale(Vec3f offset)
    {
        transform.scale += offset;
        return transform.scale;
    }

    /*Vec3f TransformComponent::getRelativePosition()
    {
        return Vec3f();
    }
    
    Vec3f TransformComponent::getRelativeScale()
    {
        return Vec3f();
    }
    
    void TransformComponent::setRelativePosition(Vec3f new_position)
    {
    }
    
    void TransformComponent::setRelativeRotation(Vec3f new_position)
    {
    }
    
    void TransformComponent::setRelativeScale(Vec3f new_position)
    {
    }
    
    Vec3f TransformComponent::translateLocally(Vec3f offset)
    {
        return Vec3f();
    }
    
    Vec3f TransformComponent::rotateLocally(Vec3f offset)
    {
        return Vec3f();
    }
    
    Vec3f TransformComponent::scaleLocally(Vec3f offset)
    {
        return Vec3f();
    }
    
    Vec3f TransformComponent::translateRelativeTo(Vec3f offset, Vec3f origin)
    {
        return Vec3f();
    }
    
    Vec3f TransformComponent::rotateRelativeTo(Vec3f offset, Vec3f origin)
    {
        return Vec3f();
    }
    
    Vec3f TransformComponent::scaleRelativeTo(Vec3f offset, Vec3f origin)
    {
        return Vec3f();
    }*/
    
    glm::mat4 TransformComponent::getLocalTransformMatrix()
    {
        const Mat4f transform_x = glm::rotate(Mat4f(1.0f),glm::radians(transform.rotation.x),Vec3f(1.0f,0.0f,0.0f));
        const Mat4f transform_y = glm::rotate(Mat4f(1.0f),glm::radians(transform.rotation.y),Vec3f(0.0f,1.0f,0.0f));
        const Mat4f transform_z = glm::rotate(Mat4f(1.0f),glm::radians(transform.rotation.z),Vec3f(0.0f,0.0f,1.0f));

        //Yaw*Pitch*Roll (Euler rotate around z then y then x assuming right hand coordinates)
        const Mat4f rotation_matrix = transform_y * transform_x * transform_z;

        //TRS matrix as this is local space
        return glm::translate(Mat4f(1.0f),transform.position) *
            rotation_matrix *
            glm::scale(Mat4f(1.0f),transform.scale);
    }
};