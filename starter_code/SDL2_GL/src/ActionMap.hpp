#pragma once

#include <SDL.h>
#include <functional>
#include <list>
#include <string>
#include <unordered_map>

namespace AppCoreGui{

using TRIGGER_CALLBACK=std::function<bool(SDL_Event)>;
using ACTION_CALLBACK=std::function<void(void)>;
using ACTION_CALLBACKS=std::list<std::function<void(void)>>;
//Can't avoid macros for convenience substitutions...
#define TRIGGER_FN [&](SDL_Event event)->bool
#define ACTION_FN [&]()

class Action{
public:
    Action();
    Action(const std::string action_name,const TRIGGER_CALLBACK trigger_fn,const ACTION_CALLBACKS action_callbacks={});
    ~Action()=default;

    void operator()();
    bool isTriggered(SDL_Event event);
    void addCallback(ACTION_CALLBACK callback);
    const std::string getName();
private:
    std::string name;
    TRIGGER_CALLBACK triggered;
    ACTION_CALLBACKS callbacks;
};

class ActionMap{
public:
    using key=SDL_KeyCode;
    static bool shiftPressed(){
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
        return state[key::SDLK_LSHIFT] || state[key::SDLK_RSHIFT];
    }
    static bool altPressed(){
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
        return state[key::SDLK_LALT] || state[key::SDLK_RALT];
    }
    static bool ctlPressed(){
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
        return state[key::SDLK_LCTRL] || state[key::SDLK_RCTRL];
    }
    static bool keyModifier(){
        return shiftPressed() || altPressed() || ctlPressed();
    }
    static bool keyPressed(key k){
        const uint8_t* state = SDL_GetKeyboardState(nullptr);
        return state[k];
    }

    ActionMap()=default;
    ~ActionMap()=default;

    void addAction(Action new_action);

    bool removeAction(Action action);
    bool removeAction(std::string action_name);

    void handleEvent(SDL_Event event);

    bool registerActionCallback(std::string action_name, ACTION_CALLBACK callback);

    void resetTriggers();

    bool actionTriggered(std::string action_name);
    
    void callAction(std::string action_name);
private:
    std::unordered_map<std::string,Action> actions;
    std::unordered_map<std::string, bool> triggered;
};

};