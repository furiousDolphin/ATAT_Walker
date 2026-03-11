

#include "ValueManager.hpp"


ValueManager::ValueManager() :
    getter{[this](){return val_;}},
    setter{[this](double new_val){dirty_ = (new_val == val_ ? false : true); val_=new_val;}}
{

}

double ValueManager::get_val() const
{return val_;}
void ValueManager::set_val(double new_val)
{ dirty_ = (new_val == val_ ? false : true); val_=new_val; }
bool ValueManager::check_and_reset_dirty()
{
    bool res = dirty_;
    dirty_ = false;
    return res;
}