#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Alias.hpp"
//#include "FloatRect.h"
#include "Entity.hpp"
#include "Transform.hpp"


namespace AppCoreGui{

    class Camera;

    //extern Camera global_view;

    class Camera:public EntityBase{
    public:
        Camera(Vec3f pos = Vec3f(0.0f,0.0f,0.0f), Vec3f target_pos=Vec3f(0.0f,0.0f,0.0f), Vec3f up = Vec3f(0.0f,1.0f,0.0f));

        static Camera& getGlobalView()
        {
            static Camera global_view(Vec3f(0.0f,0.0f,-3.0f));
            return global_view;
        }

        operator Mat4f(){
            return view;
        };

        operator Mat4f*(){
            return &view;
        };

        void setTarget(Vec3f new_target);
        void clearTarget();
        bool hasTarget();
        void recalculate();
    protected:        
        //Vec3f position;
        Vec3f target;
        Vec3f dir_target2camera;
        Vec3f up;
        Vec3f right_axis;
        Vec3f up_axis;
        Vec3f front_axis;

        Mat4f view;

        bool active;
        bool free_view;
    };

    /*class View{
    public:
        View();
        View(const FloatRect& rectangle);
        View(const Vec2f& center, const Vec2f& size);

        void setCenter(float x, float y);
        void setCenter(const Vec2f& center);

        void setSize(float width, float height);
        void setSize(const Vec2f& size);

        void setRotation(float angle_deg);
        void setViewport(const FloatRect& viewport);

        void reset(const FloatRect& rectangle);

        const Vec2f& getCenter();
        const Vec2f& getSize();

        float getRotation();

        const FloatRect& getViewport();

        void move(float offset_x, float offset_y);
        void move(const Vec2f& offset);

        void rotate(float angle_offset_deg);

        void zoom (float factor);

        const Transform& getTransform() const;
        const Transform& getInverseTransform() const;

    };*/

};