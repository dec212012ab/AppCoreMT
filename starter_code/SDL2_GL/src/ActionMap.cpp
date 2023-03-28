#include "ActionMap.hpp"

namespace AppCoreGui
{
    Action::Action()
    {
        triggered=TRIGGER_FN{return false;};
    }

    Action::Action(const std::string action_name, const TRIGGER_CALLBACK trigger_fn, const ACTION_CALLBACKS action_callbacks):
        name(action_name), triggered(trigger_fn), callbacks(action_callbacks)
    {}

    void Action::operator()()
    {
        for(const ACTION_CALLBACK& callback : callbacks){
            callback();
        }
    }

    bool Action::isTriggered(SDL_Event event)
    {
        return triggered(event);
    }

    void Action::addCallback(ACTION_CALLBACK cb)
    {
        callbacks.emplace_back(cb);
    }

    const std::string Action::getName()
    {
        return name;
    }

    void ActionMap::addAction(Action new_action)
    {
        actions[new_action.getName()] = new_action;
        triggered[new_action.getName()] = false;
    }

    bool ActionMap::removeAction(Action action)
    {
        triggered.erase(action.getName());
        return actions.erase(action.getName());
    }

    bool ActionMap::removeAction(std::string action_name)
    {
        triggered.erase(action_name);
        return actions.erase(action_name);
    }

    void ActionMap::handleEvent(SDL_Event event)
    {
        for(auto iter = actions.begin(); iter!= actions.end(); iter++){
            if(iter->second.isTriggered(event)){
                triggered[iter->first] = true;
                iter->second();
            }
        }
    }

    bool ActionMap::registerActionCallback(std::string action_name, ACTION_CALLBACK callback)
    {
        if(actions.count(action_name)<=0)return false;
        actions[action_name].addCallback(callback);
        return true;
    }

    void ActionMap::resetTriggers(){
        for(auto& item : triggered)item.second = false;
    }

    bool ActionMap::actionTriggered(std::string action_name)
    {
        return triggered[action_name];
    }

    void ActionMap::callAction(std::string action_name)
    {
        if(actions.find(action_name)!=actions.end())actions[action_name]();
    }
};