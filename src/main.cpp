#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "dbreader.h"

using namespace std;
using boost::property_tree::ptree;
using namespace Kaco;

constexpr char dbpath_app[] = "../files/config-app.db3";
constexpr char dbpath_pds2[] = "../files/config-pds2.db3";

int main(int argc, char *argv[])
{
    DBReader dbreader_app;
    dbreader_app.connect(dbpath_app);

    DBReader dbreader_pds2;
    dbreader_pds2.connect(dbpath_pds2);

    dbreader_app.dump_db((char*)"sqlite_c_dump.sql");

    return 0;
}