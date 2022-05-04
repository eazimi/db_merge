#include <iostream>
#include <tuple>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "dbreader.h"
#include "dbcompare.h"

using namespace std;
using boost::property_tree::ptree;
using namespace Kaco;

#define DEFAULT_DB
#ifdef DEFAULT_DB
constexpr char dbpath_app[] = "../files/config-app.db3";
constexpr char dbpath_pds2[] = "../files/config-psd2.db3";
#endif

#define NEW_LINE \
    cout << endl;

int main(int argc, char *argv[])
{

#ifndef DEFAULT_DB
    if (argc != 3)
    {
        cout << "-> try it like this:"
             << endl
             << "./db_merge [PATH_TO_DB1] [PATH_TO_DB2]"
             << endl;
        return 0;
    }
    auto dbpath_app = argv[1];
    auto dbpath_pds2 = argv[2];
#endif

// #define LEGACY
#ifdef LEGACY
    DbReader dbreader_app;
    bool connected = dbreader_app.connect(dbpath_app);
    if (connected)
        cout << "opened " << dbpath_app << " successfully" << endl;
    else
        cout << "can't open " << dbpath_app << endl;

    DbReader dbreader_pds2;
    connected = dbreader_pds2.connect(dbpath_pds2);
    if (connected)
        cout << "opened " << dbpath_pds2 << " successfully" << endl;
    else
        cout << "can't open " << dbpath_pds2 << endl;
#endif

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

// #define PRINT_TABLE_SCHEMA
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

// #define GET_INDICES
#ifdef GET_INDICES
    NEW_LINE;
    auto indices = dbreader_app.getIndices("metaInfo");
    cout << "indices of app::metaInfo table: " << endl;
    for (auto str : indices)
        cout << str << endl;
    NEW_LINE;
    indices = dbreader_pds2.getIndices("metaInfo");
    cout << "indices of pds2::metaInfo table: " << endl;
    for (auto str : indices)
        cout << str << endl;    
#endif

// #define GET_TRIGGERS
#ifdef GET_TRIGGERS
    NEW_LINE;
    auto triggers = dbreader_app.getTriggers("accounts");
    cout << "triggers of app::account table: " << endl;
    for (auto str : triggers)
    {
        cout << get<0>(str) << endl;
        cout << get<1>(str) << endl;
    }
    NEW_LINE;
    triggers = dbreader_pds2.getTriggers("accounts");
    cout << "triggers of pds2::account table: " << endl;
    for (auto str : triggers)
    {
        cout << get<0>(str) << endl;
        cout << get<1>(str) << endl;
    }
#endif

#define DBCOMPARE
#ifdef DBCOMPARE
    NEW_LINE;
    shared_ptr<DbReader> pdb1 = make_shared<DbReader>();
    shared_ptr<DbReader> pdb2 = make_shared<DbReader>();

    bool db_connected = pdb1->connect(dbpath_app);
    if (db_connected)
        cout << "opened " << dbpath_app << " successfully" << endl;
    else
        cout << "can't open " << dbpath_app << endl;
    
    db_connected = pdb2->connect(dbpath_pds2);
    if (db_connected)
        cout << "opened " << dbpath_pds2 << " successfully" << endl;
    else
        cout << "can't open " << dbpath_pds2 << endl;

// #define DB_ATTACH
#ifdef DB_ATTACH
    NEW_LINE;
    auto rc = pdb1->attachDb(dbpath_pds2);
    cout << endl
         << "-> attachDb" << endl
         << ((SQLITE_OK == rc) ? "true" : "false") << endl;
#endif

    unique_ptr<DbCompare> dbCompare = make_unique<DbCompare>(pdb1, pdb2);
    // unique_ptr<DbCompare> dbCompare = make_unique<DbCompare>(pdb2, pdb1);

    bool initialized = false;

#define FIRST_INIT
#ifdef FIRST_INIT
    NEW_LINE;
    cout << "-> initialize" << endl;
    initialized = dbCompare->initialize();
    cout << "initialized: " << initialized << endl;
#endif

// #define NEXT_INITS
#ifdef NEXT_INITS
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
#endif

// #define TEST_TABLES_SCHEMA
#ifdef TEST_TABLES_SCHEMA
    NEW_LINE;
    dbCompare->testTableSchema();
#endif

// #define TEST_TABLES_INDICES
#ifdef TEST_TABLES_INDICES
    NEW_LINE;
    dbCompare->testTableIndices();
#endif

// #define TEST_TABLE_TRIGGERS
#ifdef TEST_TABLE_TRIGGERS
    NEW_LINE;
    dbCompare->testTableTriggers2();    
#endif

// #define TEST_INDIVIDUAL_TABLE_TRIGGERS
#ifdef TEST_INDIVIDUAL_TABLE_TRIGGERS
    NEW_LINE;
    dbCompare->testTableTriggers2("accounts");    
#endif

// #define COMPARE_AND_MERGE
#ifdef COMPARE_AND_MERGE
    NEW_LINE;
    dbCompare->compareAndMerge();
#endif

// #define CREATE_NEW_TBL
#ifdef CREATE_NEW_TBL
    NEW_LINE;
    dbCompare->testCreateNewTbl();
#endif

#define RELEASE true
#define VER 13
#if VER == 10 || RELEASE
    dbCompare->testDbDump();
    dbCompare->testGetTables();
    dbCompare->testTableSchema();
    dbCompare->testTableIndices();
#endif
#if VER == 11 || RELEASE
    dbCompare->testDiffTableNames();
    dbCompare->testDiffTableSchemas();
#endif    
#if VER == 12 || RELEASE
    dbCompare->testTableTriggers2();
    dbCompare->testTableTriggers2("accounts");
#endif    
#if VER == 13 || RELEASE
    dbCompare->testDiffTableTriggers2();
    dbCompare->testDiffTableTriggers2("accounts");
    dbCompare->testDiffTableTriggers2("sunsModelCfg");
    dbCompare->testDiffTableTriggers2("menuTree");
#endif
#endif

    return 0;
}