

#include "ValueManager.hpp"


ValueManager::ValueManager() :
    val_{0.0},
    dirty_{false},
    getter{[this](){return val_;}},
    setter{[this](double new_val){dirty_ = (new_val == val_ ? false : true); val_=new_val;}}
{

}

void ValueManager::link_to(ValueManager& other) 
{
    this->getter = [&other]() { return other.get_val(); };
    this->setter = [&other](double v) { other.set_val(v); };
}

double ValueManager::get_val() const
{
    if (getter) 
    { return getter(); } 
    return val_;
}
    
void ValueManager::set_val(double new_val) 
{
    if (setter) 
    { setter(new_val); }
    else 
    { val_ = new_val; dirty_ = true; }
}
bool ValueManager::check_and_reset_dirty()
{
    bool res = dirty_;
    dirty_ = false;
    return res;
}

