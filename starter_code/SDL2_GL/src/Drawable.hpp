#pragma once

#include <SDL.h>
#include <SDL_opengl.h>

namespace AppCoreGui{

struct IDrawable{
    virtual void draw(SDL_Window*,SDL_GLContext*) = 0;
};

};