#include "dbreader.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <memory>

#define SEPERATOR "|"
namespace Kaco
{
    DbReader::~DbReader()
    {
        sqlite3_close(db);
    }

    bool DbReader::connect(string dbPath)
    {
        auto rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READONLY, 0);
        if (rc)
            return false;
        return true;
    }

    int DbReader::sql_exec(string sql, ExecCallback cb)
    {
        char *zErrMsg = 0;
        auto rc = sqlite3_exec(db, sql.c_str(), cb, this, &zErrMsg);

        if (SQLITE_OK != rc)
            cout << __FUNCTION__ << " returned error code " << rc << ": " << zErrMsg;

        if (zErrMsg)
            sqlite3_free(zErrMsg);

        return rc;
    }

    vector<string> DbReader::sql_exec(string cmd)
    {
        vector<string> result = {};

        auto cb = [](void *buffer, int cnt, char **row, char **cols)
        {
            stringstream ssdata;
            for (auto i = 0; i < cnt; i++)
            {
                if (i)
                    ssdata << SEPERATOR;
                if (row[i])
                    ssdata << row[i];
            }
            (*((vector<string> *)buffer)).push_back(ssdata.str());
            return 0;
        };
        char *zErrMsg = (char *)(("ERROR::" + cmd).c_str());

        auto rc = sqlite3_exec(db, cmd.c_str(), cb, (void *)&result, &zErrMsg);

        return result;
    }

    int DbReader::dbDump(char *fileName)
    {
        FILE *fp = NULL;

        sqlite3_stmt *stmt_table = NULL;
        sqlite3_stmt *stmt_data = NULL;

        const char *table_name = NULL;
        const char *data = NULL;
        int col_cnt = 0;

        int ret = 0;
        int index = 0;
        char cmd[4096] = {0};

        fp = fopen(fileName, "w");
        if (!fp)
            return -1;

        ret = sqlite3_prepare_v2(db, "SELECT sql,tbl_name FROM sqlite_master WHERE type = 'table';",
                                 -1, &stmt_table, NULL);
        if (ret != SQLITE_OK)
            goto EXIT;

        fprintf(fp, "PRAGMA foreign_keys=OFF;\nBEGIN TRANSACTION;\n");

        ret = sqlite3_step(stmt_table);
        while (ret == SQLITE_ROW)
        {
            data = (const char *)sqlite3_column_text(stmt_table, 0);
            table_name = (const char *)sqlite3_column_text(stmt_table, 1);
            if (!data || !table_name)
            {
                ret = -1;
                goto EXIT;
            }

            /* CREATE TABLE statements */
            fprintf(fp, "%s;\n", data);

            /* fetch table data */
            sprintf(cmd, "SELECT * from %s;", table_name);

            ret = sqlite3_prepare_v2(db, cmd, -1, &stmt_data, NULL);
            if (ret != SQLITE_OK)
                goto EXIT;

            ret = sqlite3_step(stmt_data);
            while (ret == SQLITE_ROW)
            {
                sprintf(cmd, "INSERT INTO %s VALUES(", table_name);
                col_cnt = sqlite3_column_count(stmt_data);
                for (index = 0; index < col_cnt; index++)
                {
                    if (index)
                        strcat(cmd, ",");
                    data = (const char *)sqlite3_column_text(stmt_data, index);

                    if (data)
                    {
                        if (sqlite3_column_type(stmt_data, index) == SQLITE_TEXT)
                        {
                            strcat(cmd, "'");
                            strcat(cmd, data);
                            strcat(cmd, "'");
                        }
                        else
                        {
                            strcat(cmd, data);
                        }
                    }
                    else
                        strcat(cmd, "NULL");
                }
                fprintf(fp, "%s);\n", cmd);
                ret = sqlite3_step(stmt_data);
            }

            ret = sqlite3_step(stmt_table);
        }

        /* Triggers */
        if (stmt_table)
            sqlite3_finalize(stmt_table);

        ret = sqlite3_prepare_v2(db, "SELECT sql FROM sqlite_master WHERE type = 'trigger';",
                                 -1, &stmt_table, NULL);
        if (ret != SQLITE_OK)
            goto EXIT;

        ret = sqlite3_step(stmt_table);
        while (ret == SQLITE_ROW)
        {
            data = (const char *)sqlite3_column_text(stmt_table, 0);
            if (!data)
            {
                ret = -1;
                goto EXIT;
            }

            /* CREATE TABLE statements */
            fprintf(fp, "%s;\n", data);

            ret = sqlite3_step(stmt_table);
        }

        fprintf(fp, "COMMIT;\n");

    EXIT:
        if (stmt_data)
            sqlite3_finalize(stmt_data);
        if (stmt_table)
            sqlite3_finalize(stmt_table);
        if (fp)
            fclose(fp);
        return ret;
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
        auto tableSchema = sql_exec(ss.str());
        return tableSchema;
    }

    string DbReader::getCreateTblSQL(string tblName)
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

    vector<pair<string, string>> DbReader::getTriggers(string tableName)
    {
        stringstream ss;
        ss << "SELECT name, sql from sqlite_master WHERE type='trigger' AND tbl_name='" << tableName << "';";
        auto result = sql_exec(ss.str());

        vector<pair<string, string>> triggers{};
        for (auto str : result)
        {
            auto pos = str.find(SEPERATOR);
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
        auto tableIndices = sql_exec(ss.str());
        return tableIndices;
    }

} // namespace Kaco