#include "Camera.hpp"

namespace AppCoreGui{

//Camera global_view(Vec3f(0.0f,0.0f,-3.0f)); // = EntityFactory::create<Camera&>(Vec3f(0.0f,0.0f,-3.0f));

Camera::Camera(Vec3f pos, Vec3f target_pos, Vec3f up):
    up(up)
{   
    getRootComponent().setPosition(pos); 
    target = target_pos;
    free_view = target == Vec3f(0.0f,0.0f,0.0f);
    recalculate();
}

void Camera::recalculate()
{
    if(free_view){
        target = getRootComponent().getPosition() + Vec3f(0.0f,0.0f,1.0f);
    }
    dir_target2camera = glm::normalize(target-getRootComponent().getPosition());
    right_axis = glm::normalize(glm::cross(up,dir_target2camera));
    up_axis = glm::cross(dir_target2camera,right_axis);
    view = glm::lookAt(getRootComponent().getPosition(),target,up);
}

void Camera::setTarget(Vec3f new_target)
{
    target = new_target;
    free_view = target == getRootComponent().getPosition();
    recalculate();
}

void Camera::clearTarget()
{
    target = Vec3f();
    free_view = true;
    recalculate();
}

bool Camera::hasTarget()
{
    return !free_view;
}

};