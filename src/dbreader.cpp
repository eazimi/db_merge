#include "dbreader.h"
#include <cstring>
#include <sstream>
#include <memory>
#include "global_funcs.hpp"
#include "dump.h"
#include "dump_builder.h"
#include "table_builder.hpp"
#include "trigger_builder.hpp"

namespace Kaco
{
    DbReader::~DbReader()
    {
        sqlite3_close(db);
    }

    bool DbReader::connect(string dbPath)
    {
        auto rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READWRITE, 0);
        if (rc)
            return false;
        return true;
    }

    int DbReader::attach_db(string dbPath, DB_IDX alias)
    {
        stringstream ss;
        ss << "ATTACH '" << dbPath << "' AS " << DB_ALIAS[alias]; 
        int rc = sql_exec(ss.str(), nullptr, nullptr);
        return rc;
    }

    int DbReader::sql_exec(string cmd, ExecCallback cb, vector<string> *results)
    {
        auto err_msg = const_cast<char *>((string("ERROR::").append(cmd)).c_str());
        int rc = sqlite3_exec(db, cmd.c_str(), cb, results, &err_msg);
        return rc;
    }

    void DbReader::dbDump(char *fileName)
    {
        Dump dump = Dump::create()
             .tables().dump_tbls(db)
             .triggers().dump_triggers(db);
        dump.save_dump(fileName);
    }

    void DbReader::command_exec(string dbPath, string output, string command)
    {
        stringstream ss;
        ss << "sqlite3 " << dbPath << " \"" << command << " \" > " << output;
        system(ss.str().c_str());
    }

    void DbReader::dbDiff(string dbPath1, string dbPath2, string output)
    {
        stringstream ss;
        ss << "sqldiff --transaction " << dbPath1 << " " << dbPath2 << " > " << output;
        system(ss.str().c_str());
    }

    vector<string> DbReader::getTables()
    {
        vector<string> tables = {};
        sqlite3_stmt *stmt_table = nullptr;
        auto query = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;";
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt_table, nullptr);
        if (rc == SQLITE_OK)
        {
            rc = sqlite3_step(stmt_table);
            while (rc == SQLITE_ROW)
            {
                auto data = (const char *)sqlite3_column_text(stmt_table, 0);
                tables.push_back(data);
                rc = sqlite3_step(stmt_table);
            }
        }
        if (stmt_table)
            sqlite3_finalize(stmt_table);
        return tables;
    }

    vector<string> DbReader::getTableSchema(string tableName)
    {
        stringstream ss;
        ss << "PRAGMA table_info(" << tableName << ");";
        vector<string> tableSchema;
        sql_exec(ss.str(), cb_sql_exec, &tableSchema);
        return tableSchema;
    }

    string DbReader::getCreateTblCmd(string tblName)
    {
        string sql = "";
        stringstream ss;     
        ss << "SELECT sql FROM sqlite_master WHERE type = 'table' AND tbl_name = '" << tblName << "'";
        sqlite3_stmt *stmt_table = nullptr;
        int rc = sqlite3_prepare_v2(db, ss.str().c_str(), -1, &stmt_table, nullptr);
        if(rc == SQLITE_OK)
        {
            rc = sqlite3_step(stmt_table);
            if (rc == SQLITE_ROW)
                sql = (const char *)sqlite3_column_text(stmt_table, 0);
        }
        if(stmt_table)
            sqlite3_finalize(stmt_table);

        return sql;
    }

    // return pair<trigger_name, trigger_sql_command>
    vector<pair<string, string>> DbReader::getTriggers(string tableName)
    {
        stringstream ss;
        ss << "SELECT name, sql from sqlite_master WHERE type='trigger' AND tbl_name='" << tableName << "';";
        vector<string> results;
        sql_exec(ss.str(), cb_sql_exec, &results);

        vector<pair<string, string>> triggers{};
        for (auto str : results)
        {
            auto pos = str.find(VAL_SEPERATOR);
            auto name = str.substr(0, pos);
            auto sql = str.substr(pos + 1);
            triggers.push_back(std::forward<pair<string, string>>({name, sql}));
        }

        return triggers;
    }

    vector<string> DbReader::getIndices(string tableName)
    {
        stringstream ss;
        ss << "SELECT name FROM sqlite_master WHERE type='index' AND tbl_name='" << tableName << "' ORDER BY name;";
        vector<string> tableIndices;
        sql_exec(ss.str(), cb_sql_exec, &tableIndices);
        return tableIndices;
    }

} // namespace Kaco