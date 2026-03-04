

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <vector>
#include <stdexcept>
#include <array>
#include <map>


#include "FileManagement.hpp"


void save_csv(const std::string& path, std::vector<std::vector<double>>& data)
{
    std::fstream file{ path, std::ios::out | std::ios::trunc };
    if( !file.is_open() )
    {
        throw( std::runtime_error( "Blad otwierania" ) );
        return;
    }

    int n_rows = 0;

    for( int col = 0; col < data.size(); col++ )
    {
        file << "col_" << std::to_string( col + 1 )
             << ( ( col+1 == data.size() ) ? "" : "," );

        if( n_rows < data[col].size() )
            n_rows = data[col].size();
    }

    for( int row = 0; row < n_rows; row++ )
    {
        file << "\n";

        for( int col = 0; col < data.size(); col++ )
        {
            file << ( ( data[col].size() < row+1 ) ? 0.0 : data[col][row] ) 
                 << ( ( col+1 == data.size() ) ? "" : "," );
        }
    }
}

void load_csv( const std::string& path, std::vector< std::vector<double> >& destination )
{
    std::fstream file{ path };
    if( !file.is_open() )
    { throw( std::runtime_error( "Blad otwarcia" ) ); }


    destination.clear();

    std::string line;

    if( !std::getline( file, line ) )
    { return; } 
    

    while( std::getline( file, line ) )
    {
        std::istringstream iss{ line };
        std::string token;
        int col = 0;

        while( std::getline( iss, token, ',' ) )
        {
            if( destination.size() <= col )
                destination.push_back( {} );

            if( !token.empty() )
                destination[ col ].push_back( std::stod( token ) );
            else
                destination[ col ].push_back( 0.0 );

            col++;
        }
        
    }
}

void save_txt( const std::string& path, std::map< std::string, double >& data )
{
    std::ofstream file{ path };
    if( !file.is_open() )
    { throw std::invalid_argument( "nie mozna otworzyc pliku" ); }

    file << std::fixed << std::setprecision(6);

    for( const auto& [ key, val ] : data )
    { file << key << " " << val << "\n"; }
}


