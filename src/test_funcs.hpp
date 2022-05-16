#ifndef TEST_FUNCS_HPP
#define TEST_FUNCS_HPP

#include <string>
#include <memory>
#include "dbcompare.h"
#include "global_funcs.hpp"
#include "global_defines.hpp"
#include "IDbReader.hpp"

using namespace std;
using namespace Kaco;

namespace Test
{
    static void test_readDbTriggers(const shared_ptr<DbCompare> &db)
    {
        auto triggers_db = db->readDbTriggers();
        print(triggers_db.first, "-> [1] all the triggers in the main db", "main");
        print(triggers_db.second, "-> [1] all the triggers in the ref db", "ref");        

        auto triggers = db->triggers_db();
        print(get<0>(triggers), "-> [2] all the triggers in the main db", "main");
        print(get<1>(triggers), "-> [2] all the triggers in the ref db", "ref");        
    }

    static void test_readSingleTblTriggers(const shared_ptr<DbCompare> &db, string table_name, bool print_sql = true)
    {
        auto tbl_triggers = db->readSingleTblTriggers(table_name);
        stringstream ss;
        ss << "-> [1] triggers of " << table_name << " table in the main db";
        print(tbl_triggers.first, ss.str(), "main", table_name, print_sql);
        ss.str("");
        ss << "-> [1] triggers of " << table_name << " table in the ref db";
        print(tbl_triggers.second, ss.str(), "ref", table_name, print_sql);

        auto triggers = db->trigger_tbl(table_name);
        ss.str("");
        ss << "-> [2] triggers of " << table_name << " table in the main db";
        print(get<0>(triggers), ss.str(), "main", table_name, print_sql);
        ss.str("");
        ss << "-> [2] triggers of " << table_name << " table in the ref db";
        print(get<1>(triggers), ss.str(), "ref", table_name, print_sql);
        ss.str("");
        ss << "-> [2] triggers of " << table_name << " table in the base db";
        print(get<2>(triggers), ss.str(), "base", table_name, print_sql);

        auto triggers_idx_main = db->trigger_tbl(table_name, DB_IDX::local);
        ss.str("");
        ss << "-> [3] triggers of " << table_name << " table in the main db";
        print(triggers_idx_main, ss.str(), "main", table_name, print_sql);
        auto triggers_idx_ref = db->trigger_tbl(table_name, DB_IDX::remote);
        ss.str("");
        ss << "-> [3] triggers of " << table_name << " table in the ref db";
        print(triggers_idx_ref, ss.str(), "ref", table_name, print_sql);
        auto triggers_idx_base = db->trigger_tbl(table_name, DB_IDX::base);
        ss.str("");
        ss << "-> [3] triggers of " << table_name << " table in the base db";
        print(triggers_idx_base, ss.str(), "base", table_name, print_sql);
    }

    static void test_diffTriggerDb(const shared_ptr<DbCompare> &db)
    {
        auto diff_m_trigger = db->diffTriggerDb();
        print(diff_m_trigger.first, "-> trigger in the main db but not in the ref db", "main", true); 
        print(diff_m_trigger.second, "-> trigger in the ref db but not in the main db", "ref", true); 
    }

    static void test_diffTriggerSingleTbl(const shared_ptr<DbCompare> &db, string table_name)
    {
        auto diff_m_trigger = db->diffTriggerSingleTbl(table_name);
        stringstream ss_trigger;
        ss_trigger << "-> [1] trigger in the main::" << table_name << " but not in the ref::" << table_name;
        print(diff_m_trigger.first, ss_trigger.str(), "main", table_name, false);
        ss_trigger.str("");
        ss_trigger << "-> [1] trigger in the ref::" << table_name << " but not in the main::" << table_name;
        print(diff_m_trigger.second, ss_trigger.str(), "ref", table_name, false); 

        auto diff_trigger = db->diffTriggerSingleTbl(table_name, DB_IDX::local, DB_IDX::remote);
        ss_trigger.str("");
        ss_trigger << "-> [2] trigger in the main::" << table_name << " but not in the ref::" << table_name;
        print(diff_m_trigger.first, ss_trigger.str(), "main", table_name, false);
        ss_trigger.str("");
        ss_trigger << "-> [2] trigger in the ref::" << table_name << " but not in the main::" << table_name;
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

    static void test_diffTblNameDb(const shared_ptr<DbCompare> &db)
    {
        auto diff_tbls = db->diffTblNameDb();
        print<vector<string>>("-> table [names] in the main db but not in the ref db", diff_tbls.first);
        print<vector<string>>("-> table [names] in the ref db but not in the main db", diff_tbls.second);
    }

    static void test_diffSchemaDb(const shared_ptr<DbCompare> &db)
    {
        auto diff_schema = db->diffSchemaDb();
        print(diff_schema, "-> difference in the schemas", "shcema in the main db", "schema in the ref db");
    }

    static void test_createTbl(const shared_ptr<DbCompare> &db)
    {
        // auto tbl_name = "inv";
        // auto tbl_name = "countrySettingCfg";
        // auto tbl_name = "accounts";
        auto tbl_name = "invCfg";
        string ct_sql = db->createTbl(tbl_name);
        string insert_sql = db->insertInto(tbl_name);
        print<vector<string>>("-> [table] create table", {ct_sql});
        print<vector<string>>("-> [table] insert into table", {insert_sql});
    }

    static void test_diff_records(const shared_ptr<DbCompare> &db)
    {
        auto tbl_name = "ledCfg";
        auto diff_recs = db->diff_records(tbl_name, DB_IDX::remote, DB_IDX::local);
        print<vector<string>>("-> records in the remote but not in the local", diff_recs);
        diff_recs = db->diff_records(tbl_name, DB_IDX::local, DB_IDX::remote);
        print<vector<string>>("-> records in the local but not in the remote", diff_recs);
    }

    static void test_table_pk(const shared_ptr<DbCompare> &db)
    {
        auto accounts_local = db->table_pk("accounts", DB_IDX::local);
        print<vector<string>>("-> accounts pk in the local db", {accounts_local});
        auto accounts_remote = db->table_pk("accounts", DB_IDX::remote);
        print<vector<string>>("-> accounts pk in the remote db", {accounts_remote});
        auto accounts_base = db->table_pk("accounts", DB_IDX::base);
        print<vector<string>>("-> accounts pk in the base db", {accounts_base});

        auto csc_local = db->table_pk("countrySettingCfg", DB_IDX::local);
        print<vector<string>>("-> countrySettingCfg pk in the local db", {csc_local});
        auto csc_remote = db->table_pk("countrySettingCfg", DB_IDX::remote);
        print<vector<string>>("-> countrySettingCfg pk in the remote db", {csc_remote});
        auto csc_base = db->table_pk("countrySettingCfg", DB_IDX::base);
        print<vector<string>>("-> countrySettingCfg pk in the base db", {csc_base});

        auto ledCfg_local = db->table_pk("ledCfg", DB_IDX::local);
        print<vector<string>>("-> ledCfg pk in the local db", {ledCfg_local});
        auto ledCfg_remote = db->table_pk("ledCfg", DB_IDX::remote);
        print<vector<string>>("-> ledCfg pk in the remote db", {ledCfg_remote});
        auto ledCfg_base = db->table_pk("ledCfg", DB_IDX::base);
        print<vector<string>>("-> ledCfg pk in the base db", {ledCfg_base});
    }

    static void test_records_status(const shared_ptr<DbCompare> &db)
    {
        auto tbl_name = "ledCfg";
        auto records = db->records_status(tbl_name);
        print<vector<string>>("-> new record in the remote", records.first); 
        print<vector<string>>("-> new record in the local", records.second); 
    }

} // namespace Test

#endif