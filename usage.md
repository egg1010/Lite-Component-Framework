```c++


#include "include/component.hpp"
#include <iostream>



struct info
{
    std::string name;
    int age;
};



struct pos
{
    int x,y;    
};

int f(int as)
{
    return 1;
}


int main()
{   


    ecs::manager ecss;

    //提前创建实体 默认创建500*1000
    //Create entity in advance.The default number of entities created is 500*1000
    ecss.append_preallocated_entities(1000*1000);

    auto entity1=ecss.create_entity();
    auto entity2=ecss.create_entity();  
    auto entity3=ecss.create_entity(); 
    auto entity4=ecss.create_entity(); 

    //获取运行信息
    //Get running information
    auto msg1=ecss.add(entity4,pos{15555,2222});


    //所有运行信息（只要有一次失败，就算作“false”。）
    //All running information (As long as there is one failure, it counts as 'false'.)
    auto msg2=ecss.get_operating_message();

    if (msg1||msg2)
    {
        std::cout<<"No error"<<std::endl;
    }
    else
    {
        std::cout<<msg1.read_messge()<<std::endl;
        std::cout<<msg2.read_messge()<<std::endl;
    }

    ecss.add(entity1,pos{15555,2222});
    ecss.add(pos{133,123},entity2);
    ecss.add(entity3,pos{18758,2678});
    
    ecss.addc(entity1,info{"name",10})
        .addc(info{"name",10},entity2)
        .addc(entity3,info{"name",10})
        .addc(entity4,info{"name",10});

    //重复添加将被直接覆盖
    //Duplicate additions will be directly overwritten
    ecss.addc(entity1,pos{55,66})
        .addc(pos{33,44},entity2)
        .addc(entity3,pos{11,12});
    
    //获得某类型容器
    //Obtain a certain type of container
    auto pos_v = ecss.get_component_vector<pos>();

    //获得某类型单一ecs
    //Obtain a single type of ECS
    auto pos_s = ecss.get_single_class_set<pos>();

    auto msg=ecss.get_operating_message();
    if(msg)
    {
        std::cout<<"No error"<<std::endl;
    }
    else
    {
        std::cout<<msg.read_messge()<<std::endl;
    }
    
    for (auto &poss : *pos_v)
    {
        auto pos1 = poss.get_ptr<pos>();
        if(pos1!=nullptr)
        {
            std::cout<<pos1->x<<" "<<pos1->y<<std::endl;
        }
        else
        {
            std::cout<<"nullptr"<<std::endl;
        }

        poss=pos{0,0};
    }


    std::cout<<"get_single_class_set"<<std::endl;

    pos_s->add(entity3,pos{15555,2222});

    auto pos2=pos_s->get_ptr<pos>(entity1);

    if(pos2!=nullptr)
    {
        std::cout<<pos2->x<<" "<<pos2->y<<std::endl;
    }
    else
    {
        std::cout<<"nullptr"<<std::endl;
    }

    for (auto &poss : *pos_s)
    {
        auto pos1 = poss.get_ptr<pos>();
        if(pos1!=nullptr)
        {
            std::cout<<pos1->x<<" "<<pos1->y<<std::endl;
        }
        else
        {
            std::cout<<"nullptr"<<std::endl;
        }
        
    }
    
    ecss.soft_remove<info>(entity1);

    ecss.soft_removec<info>(entity2)
        .soft_removec<info>(entity3)
        .soft_removec<info>(entity4);

    //删除某类型容器
    //Remove the container instance of type [X].
    ecss.delete_type_container<pos>();
    
    // 删除实体。
    // Delete entity.
    ecss.delete_entity(entity4);


    std::cout<<"endl"<<std::endl;
    return 0;
}

```
