#pragma once

#include <deque>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <sstream>

namespace AppCoreGui{

template <class T>
class ObjectPool{
public:
    ObjectPool()
    {

    }

    ObjectPool(size_t pool_block_size)
    {

    }
    virtual ~ObjectPool()
    {

    }

    size_t size() const noexcept
    {
        /*size_t out = 0;
        for(auto& b : blocks){
            out+=b.size();
        }
        return out;*/
        return full_size;
    }

    virtual T& at(size_t id)
    {
        size_t block_index = id/block_size;
        size_t space_index = id%block_size;

        if(block_index>=blocks.size() || space_index>=block_size){
            throw std::out_of_range("Invalid index provided to ObjectPool!");
        }
        if(!blocks[block_index][space_index]){
            std::stringstream ss;
            ss<<"ObjectPool at(): Provided with index that had a null value: "<<id<<std::endl;
            throw std::runtime_error(ss.str());
        }
        return *blocks[block_index][space_index];
    }

    virtual T& operator[](size_t pos)
    {
        size_t block_index = pos/block_size;
        size_t space_index = pos%block_size;

        if(block_index>=blocks.size() || space_index>=block_size){
            throw std::out_of_range("Invalid index provided to ObjectPool!");
        }
        if(!blocks[block_index][space_index]){
            std::stringstream ss;
            ss<<"ObjectPool operator[]: Provided with index that had a null value: "<<pos<<std::endl;
            throw std::runtime_error(ss.str());
        }
        return *blocks[block_index][space_index];
    }

    virtual T const& operator[](size_t pos) const
    {
        size_t block_index = pos/block_size;
        size_t space_index = pos%block_size;

        if(block_index>=blocks.size() || space_index>=block_size){
            throw std::out_of_range("Invalid index provided to ObjectPool!");
        }
        if(!blocks[block_index][space_index]){
            std::stringstream ss;
            ss<<"ObjectPool operator[]: Provided with index that had a null value: "<<pos<<std::endl;
            throw std::runtime_error(ss.str());
        }
        return *blocks[block_index][space_index];
    }

    virtual size_t addToPool(T& item)
    {
        if(free_spaces.size()>0){
            size_t allocated_space = free_spaces.begin()->first;
            free_spaces.erase(allocated_space);            
            size_t block_index = allocated_space/block_size;
            size_t space_index = allocated_space%block_size;
            blocks[block_index][space_index] = std::make_unique<T>(item);
            return allocated_space;
        }
        else{
            if(blocks.size()==0 || blocks.back().size()>=block_size){
                blocks.emplace_back(std::vector<std::unique_ptr<T>>());//Inefficient. Pre-allocate and pass empty slots to free_spaces
            }
            blocks.back().emplace_back(std::move(std::make_unique<T>(item)));
            full_size++;
            return size()-1;            
        }
    }

    virtual size_t addToPool(T&& item)
    {
        T& ref = item;
        return addToPool(ref);
    }

    virtual size_t addToPool(std::unique_ptr<T> uptr)
    {
        if(!uptr){
            throw std::invalid_argument("Cannot add null pointer to object pool!");
        }
        T& ref = *uptr.release();
        return addToPool(ref);
    }

    virtual size_t removeItem(size_t id)
    {
        size_t block_index = id/block_size;
        size_t space_index = id%block_size;
        if(block_index>=blocks.size() || space_index>=block_size){
            throw std::out_of_range("Invalid index provided to ObjectPool!");
        }
        if(blocks[block_index][space_index]){
            blocks[block_index][space_index].reset();
            free_spaces[id]=true;
        }
        return getValidSize();
    }

    virtual bool isValidIndex(size_t index)
    {
        return index>=0 && index<size() && (free_spaces.find(index)==free_spaces.end());
    }

    virtual size_t getValidSize()
    {
        return full_size - free_spaces.size();
    }

protected:
    size_t block_size = 10;
    std::deque<std::vector<std::unique_ptr<T>>> blocks;
    std::unordered_map<size_t,bool> free_spaces;
    size_t full_size = 0;
};

}