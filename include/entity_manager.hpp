#pragma once

#include "entity.hpp"
#include "id_.hpp"
#include <vector>

class entity_manager
{
private:
    id_allocation<uint32_t> id_manager_; 
    std::vector<uint32_t> version_v_;
    
    std::vector<entity> preallocated_entities_;
    size_t current_preallocated_index_ = 0;
public:
    entity_manager(){append_preallocated_entities(500*1000);}
    void append_preallocated_entities(size_t count)
    {
        size_t initial_size = preallocated_entities_.size();
        preallocated_entities_.reserve(initial_size + count);
        
        for (size_t i = 0; i < count; ++i)
        {
            uint32_t idx = id_manager_.get_id();
            if (idx >= version_v_.size()) 
            {
                version_v_.resize(idx + 1, 0);
            }
            preallocated_entities_.emplace_back(entity(idx, version_v_[idx]));
        }
    }
    entity_manager(size_t count)
    {
        append_preallocated_entities(count);
    }

    bool is_version_valid(entity entitys)
    {
        if(entitys.index_ >= version_v_.size()) return false;
        return entitys.version_ == version_v_[entitys.index_];
    }
    
    void destroy_entity(entity &entitys)
    {
        if(!is_version_valid(entitys)) return;
        id_manager_.free_id(entitys.index_);
        version_v_[entitys.index_]++;
    }
    
    entity get_entity()
    {
        if (current_preallocated_index_ < preallocated_entities_.size())
        {
            return preallocated_entities_[current_preallocated_index_++];
        }
        else
        {
            uint32_t idx = id_manager_.get_id();
            if (idx >= version_v_.size()) 
            {
                version_v_.resize(idx + 1, 0);
            }
            return entity(idx, version_v_[idx]);
        }
    }
};