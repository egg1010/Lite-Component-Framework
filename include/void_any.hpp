#pragma once
#include <type_traits>
#include <cstdint>
#include <typeinfo>
#include <cstring>
#include <atomic>
#include "type_id.hpp"
#include <optional>
#include "void_any_config.hpp"


#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif



class Stack_monitor 
{
public:
    static bool is_stack_safe(size_t required_size) 
    {
        char dummy;
        const char* stack_addr = reinterpret_cast<const char*>(&dummy);
        
#ifdef _WIN32
        // Windows 实现
        ULONG_PTR stack_low, stack_high;
        GetCurrentThreadStackLimits(&stack_low, &stack_high);
        size_t remaining = static_cast<size_t>(stack_high - reinterpret_cast<ULONG_PTR>(stack_addr));
        return remaining > (required_size * 4);  
#else
        // POSIX 实现 
        pthread_attr_t attr;
        void* stack_base;
        size_t stack_size;
        
        if (pthread_getattr_np(pthread_self(), &attr) == 0) {
            if (pthread_attr_getstack(&attr, &stack_base, &stack_size) == 0) {
                const char* stack_top = static_cast<const char*>(stack_base) + stack_size;
                size_t remaining = static_cast<size_t>(stack_top - stack_addr);
                pthread_attr_destroy(&attr);
                return remaining > (required_size * 4);
            }
            pthread_attr_destroy(&attr);
        }
        return true; 
#endif
    }
};


enum class Void_any_option
{
    Enable_stack_memory=1,
    Absolute_heap_memory=2
};




using vao=Void_any_option;

class Void_any
{
private:

    inline static constexpr size_t index_max{SINGLE_OBJECT_STACK_SIZE};
    alignas(std::max_align_t) std::byte buff_[index_max];
    bool is_in_buff_{false};
    size_t type_size_{0};

    void* ptr_={nullptr};
    int any_type_id_{-1};
    void (*deleter_)(void*){nullptr};
    Void_any_option option_=Void_any_option::Absolute_heap_memory;
public:
    Void_any() 
    : 
        ptr_(nullptr), 
        deleter_(nullptr),
        option_(Void_any_option::Absolute_heap_memory),
        any_type_id_(-1)
    {}
    Void_any(Void_any_option options) 
    : 
        ptr_(nullptr), 
        deleter_(nullptr),
        any_type_id_(-1)
    {   
        option_=options;
    }
    void set_memory_mode(Void_any_option options)
    {
        option_=options;
    }
    Void_any_option get_memory_mode()
    {
        return option_;
    }
    

    template<typename T>
    Void_any(T&& object,Void_any_option options=vao::Absolute_heap_memory)
    {
        option_=options;
        using DecayedT = std::decay_t<T>;
        type_size_=sizeof(DecayedT);
        any_type_id_=type_id::get_type_id<DecayedT>();

        if (
            type_size_ <= index_max&&
            Stack_monitor::is_stack_safe(sizeof(Void_any))&&
            option_==vao::Enable_stack_memory) 
        {
            memcpy(buff_, &object, type_size_);
            is_in_buff_ = true;
        }
        else
        {
            is_in_buff_ = false;
            ptr_ = new DecayedT(std::forward<T>(object));        
            deleter_ = [](void* p) 
            {
                delete static_cast<DecayedT*>(p);
            };
        }
    }

    ~Void_any() 
    {
        if (!is_in_buff_ && deleter_ && ptr_) 
        {
            deleter_(ptr_);
            ptr_ = nullptr;
            deleter_ = nullptr;
        } 
        else if (is_in_buff_) 
        {
            memset(buff_, 0, type_size_);
        }

    }

    Void_any(Void_any&& other) noexcept
    : 
        ptr_(other.ptr_),  
        deleter_(other.deleter_),
        type_size_(other.type_size_),
        is_in_buff_(other.is_in_buff_),
        option_(other.option_),
        any_type_id_(other.any_type_id_)
    {
        other.ptr_ = nullptr;
        other.deleter_ = nullptr;
        other.any_type_id_=-1;
        if (is_in_buff_) 
        {
            memcpy(buff_, other.buff_, type_size_);
            memset(other.buff_, 0, type_size_);
        }
    }



    Void_any& operator=(Void_any&& other) noexcept
    {
        if (this != &other) 
        {
            if (!is_in_buff_ && deleter_ && ptr_) 
            {
                deleter_(ptr_);
                ptr_ = nullptr;
                deleter_ = nullptr;
            } 
            else if (is_in_buff_) 
            {
                memset(buff_, 0, type_size_);
            }

            ptr_ = other.ptr_;
            deleter_ = other.deleter_;
            type_size_ = other.type_size_;
            is_in_buff_ = other.is_in_buff_;
            option_=other.option_;
            any_type_id_=other.any_type_id_;

            other.ptr_ = nullptr;
            other.deleter_ = nullptr;
            other.is_in_buff_ = false;
            other.option_=Void_any_option::Absolute_heap_memory;
            other.any_type_id_=-1;

            if (is_in_buff_) 
            {
                memcpy(buff_, other.buff_, type_size_);
                memset(other.buff_, 0, type_size_);
            }

        }
        return *this;
    }



    template<typename T>
    void set(T&& object,Void_any_option options = vao::Absolute_heap_memory)
    {
        option_=options;
        if (!is_in_buff_ && deleter_ && ptr_) 
        {
            deleter_(ptr_);
            ptr_ = nullptr;
            deleter_ = nullptr;
        } 
        else if (is_in_buff_) 
        {
            memset(buff_, 0, type_size_);
        }

        using DecayedT = std::decay_t<T>;

        type_size_=sizeof(DecayedT);
        any_type_id_=type_id::get_type_id<DecayedT>();

        if (type_size_ <= index_max&&
            Stack_monitor::is_stack_safe(sizeof(Void_any))&&
            option_==vao::Enable_stack_memory) 
        {
            memcpy(buff_, &object, type_size_);
            is_in_buff_ = true;
        }
        else
        {
            is_in_buff_ = false;
            ptr_ = new DecayedT(std::forward<T>(object));        
            deleter_ = [](void* p) 
            {
                delete static_cast<DecayedT*>(p);
            };
        }
    }

    template<typename T>
    T* get_ptr() noexcept
    {   
        using DT = std::decay_t<T>;
        if(type_id::get_type_id<DT>()!= any_type_id_|| any_type_id_==-1)
        {
            return nullptr;
        }

        if(is_in_buff_)
        {
            return reinterpret_cast<T*>(buff_);
        }
        else
        {
            return static_cast<T*>(ptr_);
        }
        
    }

    int get_type_id()
    {
        return any_type_id_;
    }
    Void_any(const Void_any&) = delete;
    Void_any& operator=(const Void_any&) = delete;    

    bool operator==(const Void_any& other) const
    {
        return any_type_id_ == other.any_type_id_;
    }

};

inline static Void_any null_ {};