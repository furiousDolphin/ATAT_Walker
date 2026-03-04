

#ifndef FILE_MANAGEMENT_
#define FILE_MANAGEMENT_

#include <vector>
#include <list>
#include <iterator>
#include <set>
#include <string>
#include <map>


void save_csv( const std::string& path, std::vector< std::vector<double> >& data );
void load_csv( const std::string& path, std::vector< std::vector<double> >& destination );
void save_txt( const std::string& path, std::map< std::string, double >& data );





#endif