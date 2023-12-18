#include "TransformComponent.hpp"
#include "Entity.hpp"

namespace AppCoreGui{

    TransformComponent::TransformComponent(Vec3f position, Vec3f rotation, Vec3f scale):
        transform(position,rotation,scale)
    {
        component_type_id = AppCoreGui::getComponentTypeID<TransformComponent>();
        REGISTER_TYPENAME(getComponentTypeID(), "TransformComponent");
        //model_matrix = Mat4f();
    }

    Vec3f TransformComponent::getPosition()
    {
        return transform.getPosition();
    }

    Vec3f TransformComponent::getRotation()
    {
        return transform.getRotation();
    }

    Vec3f TransformComponent::getScale()
    {
        return transform.getScale();
    }

    void TransformComponent::setPosition(Vec3f new_position)
    {
        transform.setPosition(new_position);
    }

    void TransformComponent::setRotation(Vec3f new_rotation)
    {
        transform.setRotation(new_rotation);
    }

    void TransformComponent::setScale(Vec3f new_scale)
    {
        transform.setScale(new_scale);
    }

    Vec3f TransformComponent::translate(Vec3f offset)
    {
        return transform.translate(offset);
    }

    Vec3f TransformComponent::translate(float x, float y, float z)
    {
        return transform.translate(x,y,z);
    }

    Vec3f TransformComponent::translateX(float x)
    {
        return transform.translateX(x);
    }

    Vec3f TransformComponent::translateY(float y)
    {
        return transform.translateY(y);
    }

    Vec3f TransformComponent::translateZ(float z)
    {
        return transform.translateZ(z);
    }

    Vec3f TransformComponent::rotate(Vec3f offset)
    {
        return transform.rotate(offset);
    }

    Vec3f TransformComponent::rotate(float x, float y, float z)
    {
        return transform.rotate(x,y,z);
    }

    Vec3f TransformComponent::rotateX(float x)
    {
        return transform.rotateX(x);
    }

    Vec3f TransformComponent::rotateY(float y)
    {
        return transform.rotateY(y);
    }

    Vec3f TransformComponent::rotateZ(float z)
    {
        return transform.rotateZ(z);
    }
    
    Vec3f TransformComponent::scale(Vec3f offset)
    {
        return transform.scale(offset);
    }

    Vec3f TransformComponent::scale(float x, float y, float z)
    {
        return transform.scale(x,y,z);
    }

    Vec3f TransformComponent::scaleX(float x)
    {
        return transform.scaleX(x);
    }

    Vec3f TransformComponent::scaleY(float y)
    {
        return transform.scaleY(y);
    }

    Vec3f TransformComponent::scaleZ(float z)
    {
        return transform.scaleZ(z);
    }
    
    glm::mat4 TransformComponent::getLocalTransformMatrix()
    {
        return transform;
        /*const Mat4f transform_x = glm::rotate(Mat4f(1.0f),glm::radians(transform.getRotation().x),Vec3f(1.0f,0.0f,0.0f));
        const Mat4f transform_y = glm::rotate(Mat4f(1.0f),glm::radians(transform.getRotation().y),Vec3f(0.0f,1.0f,0.0f));
        const Mat4f transform_z = glm::rotate(Mat4f(1.0f),glm::radians(transform.getRotation().z),Vec3f(0.0f,0.0f,1.0f));

        //Yaw*Pitch*Roll (Euler rotate around z then y then x assuming right hand coordinates)
        const Mat4f rotation_matrix = transform_y * transform_x * transform_z;

        //TRS matrix as this is local space
        return glm::translate(Mat4f(1.0f),transform.getPosition()) *
            rotation_matrix *
            glm::scale(Mat4f(1.0f),transform.getScale());
        */
    }
};