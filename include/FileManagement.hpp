

#ifndef FILE_MANAGEMENT_
#define FILE_MANAGEMENT_

#include <vector>
#include <list>
#include <iterator>
#include <set>
#include <string>


template <typename T, typename Iterator>
std::list<std::vector<T>> get_combinations(Iterator begin, Iterator end)
{
    if ( begin == end )
    { 
        std::list<std::vector<T>> base;
        base.push_back({}); 
        return base;    
    }

    T first_elem = *begin;

    std::list<std::vector<T>> combs_without_first = get_combinations<T>(std::next(begin), end);
    std::list<std::vector<T>> combs_with_first;

    for ( std::vector<T> comb : combs_without_first )
    {
        comb.push_back(first_elem);
        combs_with_first.push_back(comb);
    }
    combs_with_first.splice(combs_with_first.end(), combs_without_first);
    return combs_with_first;
}

struct LoadingLandData
{
    std::string path;
    std::set<char> required_sides;
    std::set<char> forbidden_sides;
};




#endif