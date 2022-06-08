#include "dump.h"
#include "dump_builder.h"
#include <fstream>

namespace Kaco
{
    DumpBuilder Dump::create()
    {
        return DumpBuilder{};
    }

    void Dump::save_dump(string dump_path)
    {
        ofstream ofs(dump_path, ios_base::out);
        ofs << oss.str();
        ofs.close();
    }

    void Dump::cleanup()
    {
        if (stmt_data)
            sqlite3_finalize(stmt_data);
        if (stmt_table)
            sqlite3_finalize(stmt_table);
        oss.str("");
    }

    bool Dump::prepare(sqlite3 *db, const string &cmd, sqlite3_stmt *stmt)
    {
        int rc = sqlite3_prepare_v2(db, cmd.c_str(), -1, &stmt, nullptr);
        bool rc_bool = (rc == SQLITE_OK); 
        if (!rc_bool)
            cleanup();
        return rc_bool;
    }

    bool Dump::table_info(string &tbl_data, string &tbl_name)
    {
        tbl_data = (const char *)(sqlite3_column_text(stmt_table, 0));
        tbl_name = (const char *)sqlite3_column_text(stmt_table, 1);
        if (tbl_data.empty() || tbl_name.empty())
        {
            cleanup();
            return false;
        }
        return true;
    }

    string Dump::table_record()
    {
        stringstream ss;
        auto col_cnt = sqlite3_column_count(stmt_data);
        for (auto index = 0; index < col_cnt; index++)
        {
            if (index)
                ss << ",";
            auto data = (const char *)sqlite3_column_text(stmt_data, index);
            if (data)
            {
                if (sqlite3_column_type(stmt_data, index) == SQLITE_TEXT)
                    ss << "'" << data << "'";
                else
                    ss << data;
            }
            else
                ss << "NULL";
        }
        return ss.str();
    }

    string Dump::table_data(string tbl_name)
    {
        /* fetch table data */
        ostringstream ss;
        int rc = sqlite3_step(stmt_data);
        while (rc == SQLITE_ROW)
        {
            auto tbl_record = table_record();
            ss << "INSERT INTO " << tbl_name
               << " VALUES(" << tbl_record << ");\n";
            rc = sqlite3_step(stmt_data);
        }
        return ss.str();
    }

    string Dump::db_triggers()
    {
        ostringstream ss;
        auto rc = sqlite3_step(stmt_table);
        while (rc == SQLITE_ROW)
        {
            auto data = (const char *)sqlite3_column_text(stmt_table, 0);
            if (!data)
            {
                cleanup();
                return "";
            }
            ss << data << ";\n";
            rc = sqlite3_step(stmt_table);
        }
        return ss.str();
    }
} // namespace Kaco