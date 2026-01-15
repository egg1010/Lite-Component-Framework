#pragma once
#include "id_.hpp"
#include <unordered_map>

class type_id
{
private:
    inline static id_allocation type_id_allocator{};
public:
    type_id()=default;
    
    template<typename T>
    static int get_type_id()
    {
        static int id = type_id_allocator.get_id();
        return id;
    }

};
