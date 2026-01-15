#pragma once
#include <string>   
#include <sstream>

class Operating_message
{
private:
    bool line_break_{false};
    bool switch_{true};
    std::string message_;
public:
    ~Operating_message()=default;

    Operating_message(bool line_break=false)
    :
        line_break_(line_break)
    {}
    
    operator bool()const 
    { 
        return switch_; 
    }
    void reset()
    {
        switch_=true;
        message_="";
    }
    void clear_message()
    {
        message_="";
    }
    void set_switch_bool(bool switchs)
    { 
        switch_=switchs; 
    }
    bool &get_switch_bool()
    {
        return switch_;
    }
    Operating_message operator+=(Operating_message&& other)
    {
        message_ += other.message_;
        if(!switch_){switch_=false;}
        line_break_=other.line_break_;
        return *this;
    }

    Operating_message& operator+=(const Operating_message& other)
    {
        message_ += other.message_;
        if(!switch_){switch_=false;}
        line_break_=other.line_break_;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const Operating_message& str)
    { 
        os << str.message_;
        return os;
    }

    std::string_view read_messge()
    {
        return message_;
    } 
    template<typename... Args>
    void write_message(bool bool_, Args&&... args_)
    {
        std::ostringstream oss;
        ((oss << std::forward<Args>(args_)), ...);
        message_ += oss.str();

        if(line_break_)
        {
           message_ += '\n';
        }
        switch_ = bool_;
    }


    Operating_message(Operating_message&& other) noexcept= default;
    Operating_message& operator=(Operating_message&& other) noexcept= default;
    Operating_message(const Operating_message& other)noexcept= default;
    Operating_message& operator=(const Operating_message& other)noexcept= default;
};