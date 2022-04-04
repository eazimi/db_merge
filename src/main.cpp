#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sqlite3.h>

using namespace std;
using boost::property_tree::ptree;

int main(int argc, char* argv[])
{
    sqlite3 *db;
    auto rc = sqlite3_open("../files/config-app.db3", &db);
    if(rc)
        cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
    else    
        cout << "Opened database successfully" << endl; 

    sqlite3_close(db);

    return 0;
}