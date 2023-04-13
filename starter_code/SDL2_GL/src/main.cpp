
#include <glad/glad.h>

#include <SDL.h>
#undef main

#include <SDL_opengl.h>


#include <iostream>
#include <string>
#include <functional>
#include <vector>

#include "Entity.hpp"
#include "Component.hpp"
#include "RenderShapes.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "util.hpp"

#define Debug(x){\
            std::cout<<x<<std::endl;\
        }

int main(int argc, char** argv)
{
    //BEGIN TEST REGION
    AppCoreGui::ObjectPool<AppCoreGui::ComponentBase> test_pool;
    std::cout<<"Object Pool [0]: "<<test_pool.addToPool<AppCoreGui::TransformComponent>(AppCoreGui::TransformComponent())<<":"<<test_pool[0]<<std::endl;
    std::cout<<"Typename: "<<AppCoreGui::getComponentTypeName(test_pool[0].getComponentTypeID())<<std::endl;
    AppCoreGui::TransformComponent& t = dynamic_cast<AppCoreGui::TransformComponent&>(test_pool[0]);
    test_pool.clear();

    AppCoreGui::ComponentBase& base = AppCoreGui::ComponentFactory::create<AppCoreGui::ComponentBase>();

    std::cout<<"component pool size: "<<AppCoreGui::component_pool.size()<<std::endl;
    std::cout<<"component address: "<<&base<<" vs "<<&AppCoreGui::component_pool.at(0)<<std::endl;

    AppCoreGui::ComponentBase& child = AppCoreGui::ComponentFactory::create<AppCoreGui::ComponentBase>();
    base.attachChild(child);

    std::cout<<"component pool size: "<<AppCoreGui::component_pool.size()<<std::endl;
    //std::cout<<base<<child<<std::endl;

    for(int i = 0; i<AppCoreGui::component_pool.size();i++){
        std::cout<<AppCoreGui::component_pool[i]<<std::endl;
    }

    base.detachChild(child);

    std::cout<<"Post-detach"<<std::endl;
    for(int i = 0; i<AppCoreGui::component_pool.size();i++){
        std::cout<<AppCoreGui::component_pool[i]<<std::endl;
    }

    AppCoreGui::component_pool.removeItem(0);//NOTE: At this point all references to item 0 are invalid and cannot be reassigned

    std::cout<<"Post-delete"<<std::endl;
    for(int i = 0; i<AppCoreGui::component_pool.size();i++){
        if(AppCoreGui::component_pool.isValidIndex(i))
            std::cout<<AppCoreGui::component_pool[i]<<std::endl;
        else std::cout<<"Invalid index: "<<i<<std::endl;
    }
    AppCoreGui::ComponentFactory::create<AppCoreGui::ComponentBase>();

    std::cout<<"Post-create"<<std::endl;
    for(int i = 0; i<AppCoreGui::component_pool.size();i++){
        if(AppCoreGui::component_pool.isValidIndex(i))
            std::cout<<AppCoreGui::component_pool[i]<<std::endl;
        else std::cout<<"Invalid index: "<<i<<std::endl;
    }
    
    //END TEST REGION

    const char* glsl_version = "#version 450";

    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_GAMECONTROLLER)){
        std::cerr<<"SDL Error: "<<SDL_GetError()<<std::endl;
        return EXIT_FAILURE;
    }

    SDL_GL_LoadLibrary(NULL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);

    SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_Window* window = SDL_CreateWindow("SDL Window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1024,1024,window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    std::cout<<"OpenGL Loaded"<<std::endl;
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    std::cout<<"Vendor: "<<glGetString(GL_VENDOR)<<std::endl;
    std::cout<<"Renderer: "<<glGetString(GL_RENDERER)<<std::endl;
    std::cout<<"Version: "<<glGetString(GL_VERSION)<<std::endl;

    SDL_GL_MakeCurrent(window,gl_context);
    SDL_GL_SetSwapInterval(1);

    glViewport(0,0,1024,1024);

    SDL_Event window_event;
    int w = 1024; int h = 1024;
    bool wireframe = false;
    bool show_vertex_colors = false;
    bool show_shaded = false;
    int swap_texture = 0;
    int last_texture = 0;

    float time_value = 0.0f;
    float green_value = 0.0f;

    AppCoreGui::RectangleShape r(1,1);
    r.create();
    r.setFillColor("../../resources/Textures/container.jpg");

    std::cout<<"Rect Pos: "<<r.getRootComponent().getPosition()<<std::endl;

    AppCoreGui::Vec3f rotation_axis(0.0f);
    while(true){
        if(SDL_PollEvent(&window_event)){
            if(window_event.type == SDL_QUIT)break;
            if(window_event.type == SDL_KEYUP){
                if(window_event.key.keysym.sym == SDLK_ESCAPE)break;
                if(window_event.key.keysym.sym == SDLK_w)wireframe = !wireframe;
                if(window_event.key.keysym.sym == SDLK_c)show_vertex_colors = !show_vertex_colors;
                if(window_event.key.keysym.sym == SDLK_s)show_shaded = !show_shaded;
                if(window_event.key.keysym.sym == SDLK_t)swap_texture++;
                if(window_event.key.keysym.sym == SDLK_r){
                    rotation_axis = AppCoreGui::Vec3f(0,0,0);
                    r.getRootComponent().setRotation(AppCoreGui::Vec3f(0,0,0));
                }
                if(window_event.key.keysym.sym == SDLK_RIGHT){
                    r.getRootComponent().translate(AppCoreGui::Vec3f(.1,0,0));
                }
                if(window_event.key.keysym.sym == SDLK_LEFT){
                    r.getRootComponent().translate(AppCoreGui::Vec3f(-.1,0,0));
                }
                if(window_event.key.keysym.sym == SDLK_x)rotation_axis = AppCoreGui::Vec3f(glm::radians(5.0f),0,0);
                if(window_event.key.keysym.sym == SDLK_y)rotation_axis = AppCoreGui::Vec3f(0,glm::radians(5.0f),0);
                if(window_event.key.keysym.sym == SDLK_z)rotation_axis = AppCoreGui::Vec3f(0,0,glm::radians(5.0f));
            }
            if(window_event.type == SDL_WINDOWEVENT){
                if(window_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                    if(SDL_GetWindowFromID(window_event.window.windowID) == window){                    
                        SDL_GetWindowSize(window,&w,&h);
                        std::cout<<"New Size: "<<w<<" "<<h<<std::endl;
                        glViewport(0,0,w,h);
                    }
                    else std::cout<<"RESIZE OTHER WINDOW?"<<std::endl;
                }
            }
        }
        float f = static_cast<float>(w)/static_cast<float>(h);
        if(f<0.0f)f = 0.0f;
        if(f>1.0f)f = 1.0f/f;
        
        glClearColor(0.2f,0.3f,f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

        
        //Draw Triangle
        time_value += 0.005;
        green_value = (sin(time_value)/2.0f) + 0.5f;

        r.getRootComponent().rotate(rotation_axis);

        r.setFillColor(0,255*green_value,0);
        
        r.setWireframeMode(wireframe);
        r.setShowVertexColors(show_vertex_colors);
        r.setShowShaded(show_shaded);

        if(swap_texture!=last_texture){
            if(swap_texture%2==0)r.setFillColor("../../resources/Textures/container.jpg");
            else r.setFillColor("../../resources/Textures/awesomeface.png");
            last_texture=swap_texture;
        }

        r.draw(window,&gl_context);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    

    return 0;
}