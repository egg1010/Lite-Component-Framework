#pragma once
#include <ostream>


struct entity
{
    entity(int id):id_(id){}
    int id_{-1};
    friend std::ostream& operator<<(std::ostream& os, const entity& e)
    {
        os << e.id_;
        return os;
    }
    
};