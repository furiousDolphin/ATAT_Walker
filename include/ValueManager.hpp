

#ifndef VALUE_MANGER_HPP_
#define VALUE_MANGER_HPP_

#include <functional>

using DoubleSetter = std::function<void(double)>;
using DoubleGetter = std::function<double(void)>;

class ValueManager
{
    public:
        ValueManager();

        double get_val() const;
        void set_val(double new_val);
        bool check_and_reset_dirty();
        void link_to(ValueManager& other);

        DoubleGetter getter;
        DoubleSetter setter;
    private:
        double val_;
        bool dirty_;
};

struct OscilloscopeInputs
{
    ValueManager u;
    ValueManager y;
};

#endif