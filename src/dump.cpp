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
} // namespace Kaco