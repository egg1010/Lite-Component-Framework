#pragma once

#include "entity.hpp"
#include "id_.hpp"
#include <vector>

class entity_manager
{
private:
    inline static id_allocation<int> id_manager_;
    std::vector<int> version_v_;
    
public:
    entity_manager(){}
    bool is_version_valid(entity entitys)
    {
        return entitys.version_ == version_v_[entitys.id_];
    }

    void destroy_entity(entity entitys)
    {
        id_manager_.free_id(entitys.id_);
        version_v_[entitys.id_]++;
    }
    entity get_entity()
    {
        auto id = id_manager_.get_id();
        if(id>=version_v_.size())
        {
            version_v_.resize(id+1,0);
            version_v_[id]=0;
            return entity{id,0};
        } 
        else
        {
            return entity{id,version_v_[id]};
        }
    }

};