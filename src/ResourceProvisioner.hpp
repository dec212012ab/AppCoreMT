#ifndef __RESOURCEPROVISIONER_HPP__
#define __RESOURCEPROVISIONER_HPP__

#include <any>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

#include "SafeVar.hpp"

namespace AppCore{

class SharedResource{
public:
	using Ptr = std::shared_ptr<SharedResource>;
	
	SharedResource():owning_thread_id(""),variable_name(""),var(0){};

	template<typename T>
	SharedResource(std::string owner, std::string var_name, T&& val)
	:owning_thread_id(owner), variable_name(var_name), var(val)
	{
	}

	SharedResource(std::string owner, std::string var_name, const char* val)
	:owning_thread_id(owner), variable_name(var_name),var(std::string(val))
	{
	}
	
	template<typename T>
	static SharedResource::Ptr create(std::string owner, std::string var_name, T&& val){
		SharedResource::Ptr p = std::make_shared<SharedResource>(owner,var_name,val);
		return p;
	}

	SafeVar<std::any>& get(){
		return var;
	}

	template<typename T>
	T getAs(){
		return std::any_cast<T>(var.get());
	}

	template<typename T>
	T* getPtrAs(){
		return std::any_cast<T>(var.getPtr());
	}

	template<typename T>
	std::optional<T> safeGetAs(bool locking = false, int timeout_ms=-1){
		try{
			if(locking){
				if(timeout_ms<0)var.lock();
				else if(!var.tryLock(timeout_ms))return std::nullopt;
			}
			T ret = std::any_cast<T>(var.get());
			if(locking)var.unlock();
			return ret;
		}
		catch(const std::bad_any_cast& e){
			std::cerr<<"AppCore::ResourceProvisioner::safeGetAs : Bad any cast caught"<<std::endl;
			return std::nullopt;
		}
	}

	template<typename T>
	std::optional<T*> safeGetAs(bool locking = false, int timeout_ms=-1){
		try{
			if(locking){
				if(timeout_ms<0)var.lock();
				else if(!var.tryLock(timeout_ms))return std::nullopt;
			}
			T* ret = std::any_cast<T>(var.getPtr());
			if(locking)var.unlock();
			return ret;
		}
		catch(const std::bad_any_cast& e){
			std::cerr<<"AppCore::ResourceProvisioner::safeGetAs : Bad any cast caught"<<std::endl;
			return std::nullopt;
		}
	}

	std::string getOwningThreadID(){return owning_thread_id;};
	std::string getVariableName(){return variable_name;};

protected:
	friend bool operator== (const SharedResource& lhs, const SharedResource& rhs){
		return lhs.owning_thread_id == rhs.owning_thread_id && 
			lhs.variable_name == rhs.variable_name &&
			&lhs.var == &rhs.var;
	}

	friend bool operator!= (const SharedResource& lhs, const SharedResource& rhs){
		return !(lhs==rhs);
	}

private:
	std::string owning_thread_id;
	std::string variable_name;
	SafeVar<std::any> var;
};

class ResourceProvisioner{
public:
	ResourceProvisioner()=default;
	~ResourceProvisioner()=default;

	template<typename T>
	bool safeAddSharedResource(std::string thread_id, std::string variable_name, T&& val, bool overwrite=false, size_t timeout_ms=-1){
		if(timeout<0)variable_store.lock();
		else if(!variable_store.tryLock(timeout_ms))return false;
		bool ret = addSharedResource(thread_id,variable_name,val,overwrite);
		variable_store.unlock();
		return ret;
	}

	SharedResource::Ptr safeGetSharedResource(std::string thread_id, std::string variable_name, int timeout_ms=-1){
		if(timeout_ms<0)variable_store.lock();
		else if(!variable_store.tryLock(timeout_ms))return false;
		SharedResource::Ptr ret = getSharedResource(thread_id,variable_name);
		variable_store.unlock();
		return ret;
	}

	template<typename T>
	bool addSharedResource(std::string thread_id, std::string variable_name, T&& val, bool overwrite=false){
		if(overwrite){
			variable_store.get()[thread_id][variable_name] = SharedResource::create(thread_id,variable_name,val);
			return true;
		}
		else{
			if(variable_store.get()[thread_id].count(variable_name)!=0)
				return false;
			variable_store.get()[thread_id][variable_name] = SharedResource::create(thread_id,variable_name,val);
			return true;
		}
	}

	SharedResource::Ptr getSharedResource(std::string thread_id, std::string variable_name){
		if(variable_store.get().count(thread_id)==0 || variable_store.get()[thread_id].count(variable_name)==0)
			return nullptr;
		return variable_store.get()[thread_id][variable_name];
	}

private:
	SafeVar<std::map<std::string,std::map<std::string,SharedResource::Ptr>>> variable_store;
};

}
#endif
