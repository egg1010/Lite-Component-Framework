#pragma once
#include <tuple>
#include <concepts> 
#include <functional>
#include <string>
#include <unordered_map>  
#include <memory>






class fun_return_value
{
public:
    fun_return_value()=default;
    ~fun_return_value()=default;
    fun_return_value(void *return_value,std::string function_information,size_t hs)
    :
        return_value(return_value),
        function_information(function_information),
        hs_function_information(hs)
    {}

    void * return_value=nullptr;
    std::string function_information{""};
    size_t hs_function_information{0};

    
};


class function_storage_base 
{
public:
    virtual ~function_storage_base() = default;
    virtual void fun() = 0;
    virtual void *get_return_ptr(){return nullptr;}
    virtual std::string get_function_information() const = 0;
    virtual fun_return_value get_return_information(){return fun_return_value(nullptr,"",0);}

};


template<typename return_type, typename function, typename ...parameter>
class function_storage:public function_storage_base
{
private:
    std::string function_information;
    function function_;
    std::tuple<parameter...> parameters_;
    return_type return_value_;
    size_t hs_function_information;
public:

    void set_function_information(std::string_view function_informations)
    {
        function_information=function_informations;
    }

    std::string get_function_information() const override
    {
        return function_information;
    }

    function_storage()=default;
    ~function_storage()=default;
    void fun() override
    {

        return_value_= std::apply(function_, parameters_);


    }
    template<typename Func, typename... Params>
    function_storage(Func&& func, std::string_view function_information, Params&&... params)
    : 
        function_(std::forward<Func>(func)), 
        parameters_(std::forward<Params>(params)...),
        function_information(function_information)
    {
        std::hash<std::string> string_hash;
        hs_function_information=string_hash(function_information.data());
    }

    return_type &operator()()
    {
        return_value_ = std::apply(function_, parameters_);
        return return_value_;
    }
    void* get_return_ptr() override
    {
        return &return_value_;
    }

    return_type &get_return_value() const 
    {
        return return_value_;
    }

    fun_return_value get_return_information() override
    {
        return fun_return_value(&return_value_,function_information,hs_function_information);
    }
};


template<typename function, typename ...parameter>
class function_storage<void, function, parameter...> : public function_storage_base
{
private:
    std::string function_information;
    size_t hs_function_information;
    function function_;
    std::tuple<parameter...> parameters_;
public:
    void set_function_information(std::string_view function_informations)
    {
        function_information = function_informations;
    }

    std::string get_function_information() const override
    {
        return function_information;
    }

    function_storage() = default;
    ~function_storage() = default;
    void fun() override
    {
        std::apply(function_, parameters_);
    }
    
    template<typename Func, typename... Params>
    function_storage(Func&& func, std::string_view function_information, Params&&... params)
    : 
        function_(std::forward<Func>(func)), 
        parameters_(std::forward<Params>(params)...),
        function_information(function_information)
    {
        std::hash<std::string> string_hash;
        hs_function_information=string_hash(function_information.data());
    }
    void operator()()
    {
        std::apply(function_, parameters_);
    }

    fun_return_value get_return_information() override
    {

        return fun_return_value(nullptr,function_information,hs_function_information);
    }
};

template<typename Func, typename... Params>
function_storage(Func&& func, std::string_view, Params&&...) 
    -> function_storage<
        std::decay_t<decltype(std::declval<Func>()(std::declval<Params>()...))>,
        std::decay_t<Func>, 
        std::decay_t<Params>...
    >;


template<typename T>
concept function_storage_type = std::is_base_of_v<function_storage_base, T>;




namespace event_update_separation
{
    template<typename messg>
    class Callback_manager
    {
    protected:
        std::unordered_map<messg,std::vector<std::unique_ptr<function_storage_base>>> callback_map_;
        std::vector<messg> message_v_;
        std::vector<fun_return_value>return_value_v_;
    public:
        template<function_storage_type... function_storage_s>
        constexpr void register_callback(messg messgs, function_storage_s&&... funs)
        {
            auto new_vector = std::vector<std::unique_ptr<function_storage_base>>{};
            new_vector.reserve(sizeof...(funs));
            
            ((new_vector.emplace_back(std::make_unique<std::remove_cvref_t<function_storage_s>>(std::forward<function_storage_s>(funs)))),...);
            
            callback_map_[messgs] = std::move(new_vector);

        }
        void delete_register_callback(messg messgs)
        {
            callback_map_.erase(messgs);
        }
        void delete_all_register_callback()
        {
            callback_map_.clear();
            return_value_v_.clear();
        }

        void delete_return_value_vector()
        {
            return_value_v_.clear();
        }


        void receive_message(messg message)
        {
            message_v_.push_back(message);
        }
        void call_function() noexcept
        {

            if(message_v_.empty())
            {
                return;
            }

            for(const auto& message:message_v_)
            {
                auto it=callback_map_.find(message);
                if(it!=callback_map_.end())
                {   
                    for (auto& fun:it->second)
                    {
                        fun->fun();
                        return_value_v_.push_back(
                            fun->get_return_information()
                        ); 
                    }
                }
            }


            message_v_.clear();
        }
        std::vector<fun_return_value> *get_return_value_v()
        {
            return &return_value_v_;
        }
        Callback_manager()=default;
        ~Callback_manager()=default;

    };
}
namespace eus=event_update_separation;







namespace event_message_Non_separation
{
    
    template<typename messg>
    class Callback_manager
    {
    protected:
        std::unordered_map<messg,std::vector<std::unique_ptr<function_storage_base>>> callback_map_;
        std::vector<fun_return_value>return_value_v_;
    public:
        template<function_storage_type... function_storage_s>
        constexpr void register_callback(messg messgs, function_storage_s&&... funs)
        {
            auto new_vector = std::vector<std::unique_ptr<function_storage_base>>{};
            new_vector.reserve(sizeof...(funs));
            
            ((new_vector.emplace_back(std::make_unique<std::remove_cvref_t<function_storage_s>>(std::forward<function_storage_s>(funs)))), ...);
            
            callback_map_[messgs] = std::move(new_vector);
        }
        void delete_register_callback(messg condition_variable)
        {
            callback_map_.erase(condition_variable);
        }
        void delete_all_register_callback()
        {
            callback_map_.clear();
        }
        
        void receive_message_call_function(messg message) noexcept
        {
            auto it=callback_map_.find(message);
            if(it!=callback_map_.end())
            {
                for (auto& fun:it->second)
                {
                    fun->fun();
                    return_value_v_.push_back(
                        fun->get_return_information()
                    ); 
                }
            }
        }
        decltype(auto) get_return_value_v()
        {
            return return_value_v_;
        }
        Callback_manager()=default;
        ~Callback_manager()=default;

    };



}namespace emns=event_message_Non_separation;