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

    dbreader_app.dump_db((char *)"sqlite_c_dump.sql");

    dbreader_app.run_db_command("../files/config-app.db3", "dump_accounts.sql", ".dump accounts");
    dbreader_app.run_db_command("../files/config-app.db3", "schema_accounts.sql", ".schema accounts");
    dbreader_app.run_db_command("../files/config-app.db3", "schema_config-app.sql", ".schema");

    dbreader_app.get_db_differences("../files/config-app.db3", "../files/config-pds2.db3", "diff.sql");

    auto tables = dbreader_app.get_dbTables();
    cout << "tables of config-app: " << endl;
    for(auto str:tables)
        cout << "   " << str << endl;
    
    return 0;
}