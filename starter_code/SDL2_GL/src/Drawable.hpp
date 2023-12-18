#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include "Alias.hpp"

namespace AppCoreGui{

struct IDrawable{
    virtual void draw(SDL_Window*,SDL_GLContext*,Mat4f*) = 0;
};

};