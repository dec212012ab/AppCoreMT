#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "Drawable.hpp"
#include "Entity.hpp"
#include "Alias.hpp"
#include "Shader.hpp"
#include "stb_image.h"
#include "util.hpp"
#include "Camera.hpp"

namespace AppCoreGui{

class BaseShape : public virtual DrawableEntity{
public:
    BaseShape();
    virtual ~BaseShape() = default;

    Color3 getFillColor(){return fill_color;};
    Color3 getOutlineColor(){return outline_color;};
    float getOutlineThickness(){return outline_thickness;};

    void setFillColor(Color3 new_color){fill_color = new_color;};
    void setFillColor(uint8_t r, uint8_t g, uint8_t b){fill_color = Color3(r,g,b);};
    
    //TODO: Texture wrap modes need to be introduced
    bool setFillColor(std::filesystem::path texture_path);
    
    void setOutlineColor(Color3 new_color){outline_color = new_color;};
    void setOutlineColor(uint8_t r, uint8_t g, uint8_t b){outline_color = Color3(r,g,b);};
    void setOutlineThickness(float new_thickness){outline_thickness = new_thickness;};
protected:
    Color3 fill_color;
    Color3 outline_color;
    float outline_thickness;
    static Shader default_shader;
    static Shader default_outline_shader;
    bool has_alpha_channel;
    unsigned int texture;
    bool texture_loaded;
    bool texture_refresh;
    std::filesystem::path texture_path;
    //TODO texturing
};

class RectangleShape : public virtual BaseShape{
public:
    RectangleShape(float width, float height);
    RectangleShape(Point2f size);

    float getWidth(){return rect_width;};
    float getHeight(){return rect_height;};

    void setWidth(float width){rect_width = width;};
    void setHeight(float height){rect_height = height;};

    void setSize(float width, float height);
    void setSize(Point2f size);

    virtual void create() override;
    virtual void draw(SDL_Window* window, SDL_GLContext* ctx, Mat4f* active_view=nullptr) override;

protected:
    float rect_width;
    float rect_height;
};

};