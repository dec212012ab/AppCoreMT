#include "Transform.hpp"

namespace AppCoreGui{

    Transform::Transform(Vec3f position, Vec3f rotation, Vec3f scale):
        position(position),rotation(rotation),_scale(scale)
    {
    }

    Vec3f Transform::getPosition()
    {
        return position;
    }
    
    Vec3f Transform::getRotation()
    {
        return rotation;
    }

    Vec3f Transform::getScale()
    {
        return _scale;
    }

    void Transform::setPosition(Vec3f new_position)
    {
        position = new_position;
    }

    void Transform::setRotation(Vec3f new_rotation)
    {
        rotation = new_rotation;
    }

    void Transform::setScale(Vec3f new_scale)
    {
        _scale = new_scale;
    }

    Vec3f Transform::translate(Vec3f offset)
    {
        position+=offset;
        return position;
    }

    Vec3f Transform::translate(float x, float y, float z)
    {
        position += Vec3f(x,y,z);
        return position;
    }

    Vec3f Transform::translateX(float x)
    {
        position.x += x;
        return position;
    }

    Vec3f Transform::translateY(float y)
    {
        position.y += y;
        return position;
    }

    Vec3f Transform::translateZ(float z)
    {
        position.z += z;
        return position;
    }

    Vec3f Transform::rotate(Vec3f offset)
    {
        rotation+=offset;
        return rotation;
    }

    Vec3f Transform::rotate(float x, float y, float z)
    {
        rotation += Vec3f(x,y,z);
        return rotation;
    }

    Vec3f Transform::rotateX(float x)
    {
        rotation.x += x;
        return rotation;
    }

    Vec3f Transform::rotateY(float y)
    {
        rotation.y += y;
        return rotation;
    }
    
    Vec3f Transform::rotateZ(float z)
    {
        rotation.z += z;
        return rotation;
    }

    Vec3f Transform::scale(Vec3f offset)
    {
        _scale+=offset;
        return _scale;
    }

    Vec3f Transform::scale(float x, float y, float z)
    {
        _scale += Vec3f(x,y,z);
        return _scale;
    }
    
    Vec3f Transform::scaleX(float x)
    {
        _scale.x += x;
        return _scale;
    }
    
    Vec3f Transform::scaleY(float y)
    {
        _scale.y += y;
        return _scale;
    }
    
    Vec3f Transform::scaleZ(float z)
    {
        _scale.z += z;
        return _scale;
    }
    
    Transform::operator Mat4f()
    {
        const Mat4f transform_x = glm::rotate(Mat4f(1.0f),glm::radians(getRotation().x),Vec3f(1.0f,0.0f,0.0f));
        const Mat4f transform_y = glm::rotate(Mat4f(1.0f),glm::radians(getRotation().y),Vec3f(0.0f,1.0f,0.0f));
        const Mat4f transform_z = glm::rotate(Mat4f(1.0f),glm::radians(getRotation().z),Vec3f(0.0f,0.0f,1.0f));

        //Yaw*Pitch*Roll (Euler rotate around z then y then x assuming right hand coordinates)
        const Mat4f rotation_matrix = transform_y * transform_x * transform_z;

        //TRS matrix as this is local space
        return glm::translate(Mat4f(1.0f),getPosition()) *
            rotation_matrix *
            glm::scale(Mat4f(1.0f),getScale());
    }
}