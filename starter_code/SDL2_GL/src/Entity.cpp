#pragma once

#include "Entity.hpp"


namespace AppCoreGui{
    size_t registered_entity_types = 0;
    ObjectPool<EntityBase> entity_pool;

    EntityBase::EntityBase()
    {
        pooled = false;
        has_parent=false;
        parent_id=0;
        children_instance_ids.clear();
        root_component_id = ComponentFactory::create<TransformComponent>().getInstanceID();
    }

    EntityBase::~EntityBase()
    {
        destroy();
    }

    void EntityBase::destroy()
    {
        if(pooled){
            //Remove from parent if applicable
            if(has_parent){
                entity_pool[parent_id].detachChildEntity(*this);
                has_parent=false;
                parent_id=0;
            }
            //Destroy all children
            std::vector<size_t> child_ids = getChildrenIDs();
            for(auto& i : child_ids){
                detachChildEntity(i);
                entity_pool.removeItem(i);
            }
        }
    }

    bool EntityBase::attachChildEntity(EntityBase& ref)
    {
        if(ref.isChildEntity())return false;
        if(!ref.pooled){
            std::cerr<<"Attempt to attach unpooled entity as a child to entity "<<id<<std::endl;
            return false;
        }
        ref.parent_id = id;
        ref.has_parent = true;
        children_instance_ids.emplace(ref.getInstanceID());
        return true;
    }

    bool EntityBase::attachChildEntity(size_t entity_instance_id)
    {
        EntityBase& ref = entity_pool[entity_instance_id];
        return attachChildEntity(ref);
    }

    bool EntityBase::detachChildEntity(EntityBase& ref)
    {
        if(&entity_pool[ref.getInstanceID()] != &ref){
            std::cerr<<"Target index does not match the index of the matching entity in the entity pool! (Memory Addresses: pool: "<<
                &entity_pool[ref.getInstanceID()] <<" vs provided: "<<&ref<<std::endl;
            return false;
        }
        ref.has_parent=false;
        ref.parent_id = 0;
        children_instance_ids.erase(ref.getInstanceID());
        return true;
    }

    bool EntityBase::detachChildEntity(size_t entity_instance_id)
    {
        EntityBase& ref = entity_pool[entity_instance_id];
        return detachChildEntity(ref);
    }

    bool EntityBase::attachChildComponent(ComponentBase& ref)
    {
        return getRootComponent().attachChild(ref);
    }

    bool EntityBase::attachChildComponent(size_t component_instance_id)
    {
        return getRootComponent().attachChild(component_instance_id);
    }

    bool EntityBase::detachChildComponent(ComponentBase& ref)
    {
        return getRootComponent().detachChild(ref);
    }

    bool EntityBase::detachChildComponent(size_t component_instance_id)
    {
        return getRootComponent().detachChild(component_instance_id);
    }

    bool EntityBase::isChildEntity() const
    {
        return has_parent;
    }

    bool EntityBase::isParentEntity() const
    {
        return children_instance_ids.size()>0;
    }

    bool EntityBase::isRootEntity() const
    {
        return !has_parent;
    }

    size_t EntityBase::getInstanceID() const
    {
        return id;
    }

    size_t EntityBase::getParentID() const
    {
        return parent_id;
    }

    std::vector<size_t> EntityBase::getChildrenIDs() const
    {
        return std::vector<size_t>(children_instance_ids.begin(),children_instance_ids.end());
    }

    size_t EntityBase::getRootComponentID() const
    {
        return root_component_id;
    }

    TransformComponent& EntityBase::getRootComponent()
    {
        return dynamic_cast<TransformComponent&>(component_pool[root_component_id]);
    }

    DrawableEntity::DrawableEntity():
        wireframe(false),show_vertex_colors(false),show_shaded(false)
    {
    }

    bool DrawableEntity::getWireframeMode()
    {
        return wireframe;
    }

    void DrawableEntity::setWireframeMode(bool b)
    {
        wireframe = b;
    }

    bool DrawableEntity::getShowVertexColors()
    {
        return show_vertex_colors;
    }

    void DrawableEntity::setShowVertexColors(bool b)
    {
        show_vertex_colors = b;
    }

    bool DrawableEntity::getShowShaded()
    {
        return show_shaded;
    }

    void DrawableEntity::setShowShaded(bool b)
    {
        show_shaded = b;
    }
};