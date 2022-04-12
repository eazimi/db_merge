#include <iostream>
#include <tuple>

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

// #define DUMP
#ifdef DUMP
    dbreader_app.dump_db((char *)"sqlite_c_dump.sql");
#endif

// #define RUN_DB_COMMAND
#ifdef RUN_DB_COMMAND
    dbreader_app.run_db_command("../files/config-app.db3", "dump_accounts.sql", ".dump accounts");
    dbreader_app.run_db_command("../files/config-app.db3", "schema_accounts.sql", ".schema accounts");
    dbreader_app.run_db_command("../files/config-app.db3", "schema_config-app.sql", ".schema");
#endif

// #define DB_DIFF
#ifdef DB_DIFF
    dbreader_app.get_db_differences("../files/config-app.db3", "../files/config-pds2.db3", "diff.sql");
#endif

// #define PRINT_TABLES
#ifdef PRINT_TABLES
    auto tables = dbreader_app.get_dbTables();
    cout << "tables of config-app: " << endl;
    for (auto str : tables)
        cout << str << endl;
#endif

// #define PRINT_TABLE_SCHEMA
#ifdef PRINT_TABLE_SCHEMA
    auto tableSchema = dbreader_app.getTableSchema("accounts");
    cout << "schema of accounts table: " << endl;
    for (auto str : tableSchema)
        cout << str << endl;
#endif

// #define GET_TRIGGERS
#ifdef GET_TRIGGERS
    auto triggers = dbreader_app.getTriggers("accounts");
    cout << "triggers of account table: " << endl;
    for (auto str : triggers)
    {
        cout << get<0>(str) << endl;
        cout << get<1>(str) << endl;
    }
#endif

#define GET_INDICES
#ifdef GET_INDICES
    auto indices = dbreader_app.getIndices("metaInfo");
    cout << "indices of metaInfo table: " << endl;
    for (auto str : indices)
        cout << str << endl;
#endif

    return 0;
}