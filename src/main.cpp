#include <iostream>
#include <tuple>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "dbreader.h"
#include "dbcompare.h"

using namespace std;
using boost::property_tree::ptree;
using namespace Kaco;

constexpr char dbpath_app[] = "../files/config-app.db3";
constexpr char dbpath_pds2[] = "../files/config-pds2.db3";

#define NEW_LINE \
    cout << endl;

int main(int argc, char *argv[])
{
    DBReader dbreader_app;
    dbreader_app.connect(dbpath_app);

    DBReader dbreader_pds2;
    dbreader_pds2.connect(dbpath_pds2);

// #define DUMP
#ifdef DUMP
    dbreader_app.dbDump((char *)"sqlite_c_dump.sql");
#endif

// #define RUN_DB_COMMAND
#ifdef RUN_DB_COMMAND
    dbreader_app.command_exec("../files/config-app.db3", "dump_accounts.sql", ".dump accounts");
    dbreader_app.command_exec("../files/config-app.db3", "schema_accounts.sql", ".schema accounts");
    dbreader_app.command_exec("../files/config-app.db3", "schema_config-app.sql", ".schema");
#endif

// #define DB_DIFF
#ifdef DB_DIFF
    dbreader_app.dbDiff("../files/config-app.db3", "../files/config-pds2.db3", "diff.sql");
#endif

// #define PRINT_TABLES
#ifdef PRINT_TABLES
    NEW_LINE;
    auto tables = dbreader_app.getTables();
    cout << "tables of config-app: " << endl;
    for (auto str : tables)
        cout << str << endl;
#endif

#define PRINT_TABLE_SCHEMA
#ifdef PRINT_TABLE_SCHEMA
    NEW_LINE;
    auto tableSchema = dbreader_app.getTableSchema("accounts");
    cout << "schema of app::accounts table: " << endl;
    for (auto str : tableSchema)
        cout << str << endl;
    NEW_LINE;
    tableSchema = dbreader_pds2.getTableSchema("accounts");
    cout << "schema of pds2::accounts table: " << endl;
    for (auto str : tableSchema)
        cout << str << endl;
#endif

// #define GET_TRIGGERS
#ifdef GET_TRIGGERS
    NEW_LINE;
    auto triggers = dbreader_app.getTriggers("accounts");
    cout << "triggers of account table: " << endl;
    for (auto str : triggers)
    {
        cout << get<0>(str) << endl;
        cout << get<1>(str) << endl;
    }
#endif

// #define GET_INDICES
#ifdef GET_INDICES
    NEW_LINE;
    auto indices = dbreader_app.getIndices("metaInfo");
    cout << "indices of metaInfo table: " << endl;
    for (auto str : indices)
        cout << str << endl;
#endif

#define DBCOMPARE_TEST_TABLES_SCHEMA
#ifdef DBCOMPARE_TEST_TABLES_SCHEMA
    NEW_LINE;
    shared_ptr<DBReader> pdb1 = make_shared<DBReader>();
    shared_ptr<DBReader> pdb2 = make_shared<DBReader>();

    pdb1->connect(dbpath_app);
    pdb2->connect(dbpath_pds2);

    unique_ptr<DBCompare> dbCompare = make_unique<DBCompare>(pdb1, pdb2);
    
    NEW_LINE;
    cout << "first call to initialize()" << endl;
    bool initialized = dbCompare->initialize();
    cout << "initialized: " << initialized << endl;

    // second call
    NEW_LINE;
    cout << "second call to initialize()" << endl;
    initialized = dbCompare->initialize();
    cout << "initialized: " << initialized << endl;

    // third call
    NEW_LINE;
    cout << "third call to initialize()" << endl;
    initialized = dbCompare->initialize();
    cout << "initialized: " << initialized << endl;

    NEW_LINE;
    dbCompare->testTableSchema();

#endif

    return 0;
}