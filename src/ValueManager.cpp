

#include "ValueManager.hpp"

ValueManager::ValueManager() :
    getter{[this](){return val_;}},
    setter{[this](double new_val){val_=new_val;}}
{

}

double ValueManager::get_val() const
{return val_;}
void ValueManager::set_val(double new_val)
{val_=new_val;}