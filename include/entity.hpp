#pragma once

struct entity
{

    entity(int id,int version):id_(id),version_(version){}
    int id_{-1};
    int version_{0};


};