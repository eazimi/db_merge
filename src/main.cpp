#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "dbreader.h"

using namespace std;
using boost::property_tree::ptree;
using namespace Kaco;

constexpr char dbpath_app[] = "../files/config-app.db3";
constexpr char dbpath_pds2[] = "../files/config-pds2.db3";

int main(int argc, char* argv[])
{
    DBReader dbreader_app;
    dbreader_app.connect(dbpath_app);

    DBReader dbreader_pds2;
    dbreader_pds2.connect(dbpath_pds2);

    auto rc = dbreader_app.sqlExec("select * from sqlite_master where tbl_name = 'accounts';", nullptr);
    cout << __FUNCTION__ << "(" << __LINE__ << "): " << "sqlExec(): rc = " << (rc == SQLITE_OK ? "TRUE" : "FALSE") << endl;

    return 0;
}