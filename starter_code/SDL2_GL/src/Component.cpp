#include "Component.hpp"

namespace AppCoreGui{

    size_t registered_component_types = 0;
    ObjectPool<ComponentBase> component_pool;
    std::unordered_map<size_t,std::string> typename_lut;

    std::string getComponentTypeName(size_t component_type_id) {
        try {
            std::string out = typename_lut[component_type_id];
            if (out.empty()) {
                std::stringstream ss;
                ss << "unknown (component type id: " << component_type_id;
                return ss.str();
            }
            return out;
        }
        catch (const std::out_of_range& e) {
            std::stringstream ss;
            ss << "unknown (component type id: " << component_type_id;
            return ss.str();
        }
    }

    ComponentBase::ComponentBase()
    {
        pooled = false;
        has_parent = false;
        parent_id = 0;
        children_instance_ids.clear();
        component_type_id = AppCoreGui::getComponentTypeID<ComponentBase>();
        REGISTER_TYPENAME(getComponentTypeID(), "ComponentBase");
    }

    ComponentBase::~ComponentBase()
    {
        destroy();
    }

    void ComponentBase::destroy()
    {
        //Remove from parent if applicable
        if(pooled){
            if(has_parent){
                //Verify that parent exists in pool else it's the coder's problem for using manual instances
                component_pool[parent_id].detachChild(*this);
                has_parent = false;
                parent_id = 0;
            }
            //Destroy all children
            std::vector<size_t> child_ids = getChildrenIDs();
            for(auto& i : child_ids){
                detachChild(i);
                component_pool.removeItem(i);
            }
        }
    }

    bool ComponentBase::attachChild(ComponentBase& ref)
    {
        if(ref.isChildComponent())return false;
        if(!ref.pooled){
            std::cerr<<"Attempt to attach unpooled component as a child to component "<<id<<std::endl;
            return false;
        }
        ref.parent_id = id;
        ref.has_parent = true;
        children_instance_ids.emplace(ref.getInstanceID());
        return true;
    }
    
    bool ComponentBase::attachChild(size_t component_instance_id)
    {
        ComponentBase& ref = component_pool[component_instance_id];
        return attachChild(ref);
    }

    bool ComponentBase::detachChild(ComponentBase& ref)
    {
        //Verify that the child is actually attached to the current component
        /*bool found = false;
        size_t found_pool_index = 0;
        for(auto& i : children_instance_ids){
            if(&component_pool[i] == &ref){
                found = true;
                found_pool_index = i;
                break;
            }
        }
        if(!found){
            std::cerr<<"Attempt to detach a non-child from component "<<id<<std::endl;
            return false;
        }*/
        //if(found_pool_index != ref.getInstanceID()){
        if(&component_pool[ref.getInstanceID()]!=&ref){
            std::cerr<<"Target index does not match the index of the matching component in the component pool! (Memory Addresses: pool: "<<
                &component_pool[ref.getInstanceID()]<<" vs provided: "<<&ref<<std::endl;
            return false;
        }
        ref.has_parent=false;
        ref.parent_id=0;
        //children_instance_ids.erase(found_pool_index);
        children_instance_ids.erase(ref.getInstanceID());
        return true;
    }

    bool ComponentBase::detachChild(size_t component_instance_id)
    {
        ComponentBase& ref = component_pool[component_instance_id];
        return detachChild(ref);
    }

    bool ComponentBase::isChildComponent() const
    {
        return has_parent;
    }

    bool ComponentBase::isParentComponent() const
    {
        return children_instance_ids.size()>0;
    }

    bool ComponentBase::isRootComponent() const
    {
        return !has_parent;
    }

    size_t ComponentBase::getInstanceID() const
    {
        return id;
    }

    size_t ComponentBase::getParentID() const
    {
        return parent_id;
    }

    size_t ComponentBase::getComponentTypeID() const
    {
        return component_type_id;
    }

    std::vector<size_t> ComponentBase::getChildrenIDs() const
    {
        return std::vector<size_t>(children_instance_ids.begin(),children_instance_ids.end());
    }

    std::ostream& operator<<(std::ostream& output, const ComponentBase& component)
    {
        output<<"Component ID: "<<component.id<<"\n"<<
            "\tisRoot: "<<component.isRootComponent()<<"\n"<<
            "\tisChild: "<<component.isChildComponent()<<"\n"<<
            "\tisParent: "<<component.isParentComponent()<<"\n"<<
            "\tParent ID: "<<component.getParentID()<<"\n"<<
            "\tChildren IDs: [ ";
        for(auto& id : component.getChildrenIDs()){
            output<<id<<" ";
        }
        output<<"]\n";
        return output;
    }
};