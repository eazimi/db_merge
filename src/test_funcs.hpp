#ifndef TEST_FUNCS_HPP
#define TEST_FUNCS_HPP

#include <string>
#include <memory>
#include "dbcompare.h"
#include "global_funcs.hpp"
#include "IDbReader.hpp"

using namespace std;
using namespace Kaco;

namespace Test
{
    static void test_readDbTriggers(const shared_ptr<DbCompare> &db)
    {
        auto triggers_db = db->readDbTriggers();
        print(triggers_db.first, "-> all the triggers in the main db", "main");
        print(triggers_db.second, "-> all the triggers in the ref db", "ref");        
    }

    static void test_readSingleTblTriggers(const shared_ptr<DbCompare> &db, string table_name, bool print_sql = true)
    {
        auto tbl_triggers = db->readSingleTblTriggers(table_name);
        stringstream ss;
        ss << "-> triggers of " << table_name << " table in the main db";
        print(tbl_triggers.first, ss.str(), "main", table_name, print_sql);
        ss.str("");
        ss << "-> triggers of " << table_name << " table in the ref db";
        print(tbl_triggers.second, ss.str(), "ref", table_name, print_sql);
    }

    static void test_diffTriggerDb(const shared_ptr<DbCompare> &db)
    {
        auto diff_m_trigger = db->diffTriggerDb();
        print(diff_m_trigger.first, "-> trigger in the main db but not in the ref db", "main", false); 
        print(diff_m_trigger.second, "-> trigger in the ref db but not in the main db", "ref", false); 
    }

    static void test_diffTriggerSingleTbl(const shared_ptr<DbCompare> &db, string table_name)
    {
        auto diff_m_trigger = db->diffTriggerSingleTbl(table_name);
        stringstream ss_trigger;
        ss_trigger << "-> trigger in the main::" << table_name << " but not in the ref::" << table_name;
        print(diff_m_trigger.first, ss_trigger.str(), "main", table_name, false);
        ss_trigger.str("");
        ss_trigger << "-> trigger in the ref::" << table_name << " but not in the main::" << table_name;
        print(diff_m_trigger.second, ss_trigger.str(), "ref", table_name, false); 
    }

    static void test_updateTriggerSingleTbl(const shared_ptr<DbCompare> &db, string table_name)
    {
        test_readSingleTblTriggers(db, table_name, false);
        test_diffTriggerSingleTbl(db, table_name);
        auto updated_triggers = db->updateTriggerSingleTbl(table_name);
        stringstream ss;
        ss << "-> updated trigger of table " << table_name;
        print(updated_triggers, ss.str(), "main|ref", table_name, true);
    }

    static void test_readDbTables(const shared_ptr<DbCompare> &db)
    {
        auto tables = db->readDbTables();
        print<vector<string>>("-> tables in main db", tables.first);
        print<vector<string>>("-> tables in ref db", tables.second);
    }

    static void test_readDbTblSchema(const shared_ptr<DbCompare> &db)
    {
        auto db_tbl_schema = db->readDbTblSchema();
        print(db_tbl_schema.first, "-> main db, all the table schemas");
        print(db_tbl_schema.second, "-> ref db, all the table schemas");
    }
    
} // namespace Test

#endif