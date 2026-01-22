#pragma once
#include <cstdint>
#include <atomic>
#include <vector>
#include <stack>
#include <mutex>

class Id_allocation
{
private:
    std::atomic<size_t> next_id_{1};
    mutable std::mutex recycled_mutex_;
    std::stack<size_t, std::vector<size_t>> recycled_ids_;
public:
    size_t get_id()
    {
        {
            std::lock_guard<std::mutex> lock(recycled_mutex_);
            if (!recycled_ids_.empty()) 
            {
                size_t id = recycled_ids_.top();
                recycled_ids_.pop();
                return id;
            }
        }
        
        return next_id_.fetch_add(1);
    }
    
    void free_id(size_t id)
    {
        if (id != 0) 
        {
            std::lock_guard<std::mutex> lock(recycled_mutex_);
            recycled_ids_.push(id);
        }
    }
    
    size_t total_number_of_ids() const 
    { 
        std::lock_guard<std::mutex> lock(recycled_mutex_);
        return recycled_ids_.size(); 
    }
    
    size_t maximum_id() const 
    { 
        return next_id_.load() - 1; 
    }
};

