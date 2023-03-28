#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <unordered_map>
#include "ObjectPool.hpp"

namespace AppCoreGui{

#define REGISTER_TYPENAME(type_id,name) typename_lut[type_id]=name;

class ComponentBase;

extern size_t registered_component_types;
extern ObjectPool<ComponentBase> component_pool;
extern std::unordered_map<size_t, std::string> typename_lut;

template<typename T>
size_t getComponentTypeID(){
    if(std::is_base_of<ComponentBase,T>()){
        static size_t id = ++registered_component_types;
        return id;
    }
    else return 0;
}

std::string getComponentTypeName(size_t component_type_id);

class IComponent{
public:
    virtual void update()=0;
    virtual void destroy()=0;
};

class ComponentBase : public virtual IComponent{
public:

    ComponentBase();
    virtual ~ComponentBase();
    virtual void update() override{};
    virtual void destroy() override;
    virtual bool attachChild(ComponentBase& ref);
    virtual bool attachChild(size_t component_instance_id);
    virtual bool detachChild(ComponentBase& ref);
    virtual bool detachChild(size_t component_instance_id);
    
    bool isChildComponent() const;
    bool isParentComponent() const;
    bool isRootComponent() const;

    size_t getInstanceID() const;
    size_t getParentID() const;
    size_t getComponentTypeID() const;
    std::vector<size_t> getChildrenIDs() const;

    friend std::ostream& operator<<(std::ostream& output, const ComponentBase& component);
    friend class ComponentFactory;
    friend class EntityFactory;
protected:
    bool pooled;
    bool has_parent;
    size_t parent_id;
    size_t id;
    std::set<size_t> children_instance_ids;
    size_t entity_id;
    size_t component_type_id;
};

class ComponentFactory{
public:
    template<typename T, typename... Args>
    static T& create(Args&&... args)
    {
        if(!std::is_base_of<ComponentBase,T>()){
            throw std::invalid_argument("The provided type is not derived from class ComponentBase!");
        }
        size_t id = component_pool.addToPool(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
        component_pool[id].id = id;
        component_pool[id].pooled = true;
        try{
            return dynamic_cast<T&>(component_pool[id]);
        }
        catch(const std::bad_cast& e){
            std::cout<<e.what()<<std::endl;
            std::cout<<"ComponentBase: "<<getComponentTypeID<ComponentBase>()<<" "<<getComponentTypeID<TransformComponent>()<<std::endl;
            std::cout << "Typename: " << getComponentTypeName(0)<<" "<<getComponentTypeName(1)<<" "<<getComponentTypeName(2) << std::endl;
            exit(1);
        }
    }
private:
    ComponentFactory()=default;
};

};