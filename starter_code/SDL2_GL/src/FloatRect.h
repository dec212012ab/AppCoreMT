#pragma once

#include "Alias.hpp"

using namespace AppCoreGui;

class FloatRect{
    public:
        FloatRect():_data(Vec4f()){};
        FloatRect(float rect_left, float rect_top, float width, float height):_data(Vec4f(rect_left,rect_top,width,height)){};
        FloatRect(Vec2f pos, Vec2f size):_data(pos.x,pos.y,size.x,size.y){};

        inline bool contains(float x, float y) const{
            return (x>=_data.x && x<=(_data.x+_data.z)) && (y>=_data.y && y<=(_data.y+_data.a));
        };

        inline bool contains(Vec2f point) const{
            return (point.x>=_data.x && point.x<=(_data.x+_data.z)) && (point.y>=_data.y && point.y<=(_data.y+_data.a));
        };

        bool intersects(const FloatRect& rect) const{
            //TODO: Check implementation for correctness
            Vec2f top_left, top_right, bottom_left, bottom_right;
            top_left = rect.getPos();
            top_right = Vec2f(top_left.x + rect.getWidth(),top_left.y);
            bottom_left = Vec2f(top_left.x,top_left.y - rect.getHeight());
            bottom_right = Vec2f(bottom_left.x + rect.getWidth(),bottom_left.y);

            bool a = contains(top_left) || contains(top_right) || contains(bottom_left) || contains(bottom_right);
            if(a)return true;
            
            top_left = getPos();
            top_right = Vec2f(top_left.x + getWidth(),top_left.y);
            bottom_left = Vec2f(top_left.x,top_left.y - getHeight());
            bottom_right = Vec2f(bottom_left.x + getWidth(),bottom_left.y);

            bool b = rect.contains(top_left) || rect.contains(top_right) || rect.contains(bottom_left) || rect.contains(bottom_right);
            if(b)return true;

            return a||b;
        };

        inline Vec2f getPos() const{
            return Vec2f(_data[0],_data[1]);
        }
        
        inline Vec2f getDims() const{
            return Vec2f(_data[2],_data[3]);
        }

        inline float getWidth() const{
            return _data[2];
        }
        
        inline float getHeight() const{
            return _data[3];
        }

    private:
        Vec4f _data;
    };