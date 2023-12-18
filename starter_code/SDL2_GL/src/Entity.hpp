#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <type_traits>
#include <vector>
#include "Drawable.hpp"
#include "ObjectPool.hpp"
#include "Component.hpp"
#include "TransformComponent.hpp"
#include "Alias.hpp"

namespace AppCoreGui {

//extern Camera global_view;//Default View

class EntityBase;

extern size_t registered_entity_types;
extern ObjectPool<EntityBase> entity_pool;

class IEntity{
public:
    virtual void create()=0;
    virtual void update()=0;
    virtual void destroy()=0;    
protected:
private:
};

class EntityBase : public virtual IEntity{
public:
    EntityBase();
    virtual ~EntityBase();
    virtual void create() override{};
    virtual void update() override{};
    virtual void destroy() override;

    virtual bool attachChildEntity(EntityBase& ref);
    virtual bool attachChildEntity(size_t entity_instance_id);
    virtual bool detachChildEntity(EntityBase& ref);
    virtual bool detachChildEntity(size_t entity_instance_id);

    virtual bool attachChildComponent(ComponentBase& ref);
    virtual bool attachChildComponent(size_t component_instance_id);
    virtual bool detachChildComponent(ComponentBase& ref);
    virtual bool detachChildComponent(size_t component_instance_id);

    bool isChildEntity() const;
    bool isParentEntity() const;
    bool isRootEntity() const;

    size_t getInstanceID() const;
    size_t getParentID() const;
    std::vector<size_t> getChildrenIDs() const;
    
    size_t getRootComponentID() const;
    TransformComponent& getRootComponent();

    friend class EntityFactory;
protected:
    bool pooled;
    bool has_parent;
    size_t parent_id;
    size_t id;
    std::set<size_t> children_instance_ids;

    size_t root_component_id;
};

class DrawableEntity : public virtual EntityBase, public virtual IDrawable{
public:
    DrawableEntity();
    virtual ~DrawableEntity()=default;
    virtual void draw(SDL_Window*,SDL_GLContext*,Mat4f* active_view) override {};

    bool getWireframeMode();
    void setWireframeMode(bool b);

    bool getShowVertexColors();
    void setShowVertexColors(bool b);

    bool getShowShaded();
    void setShowShaded(bool b);
    
protected:
    //GL Specific
    std::vector<uint32_t> VAOs;
    std::vector<size_t> num_elements;
    bool wireframe; //Replace with display mode: filled, wireframe, shaded
    bool show_vertex_colors;
    bool show_shaded;
};

//For RTTI (?)
template<typename T>
size_t getEntityTypeID(){
    if(std::is_base_of<EntityBase,T>()){
        static size_t id = ++registered_entity_types;
        return id;
    }
    else return 0;
}

class EntityFactory{
public:
    template<typename T, typename... Args>
    static T& create(Args&&... args)
    {
        if(!std::is_base_of<EntityBase,T>()){
            throw std::invalid_argument("The provided type is not derived from class EntityBase!");
        }
        size_t id = entity_pool.addToPool(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
        entity_pool[id].id = id;
        component_pool[entity_pool[id].root_component_id].entity_id = id;
        entity_pool[id].pooled = true;
        return entity_pool[id];
    }
private:
    EntityFactory()=default;
};

};