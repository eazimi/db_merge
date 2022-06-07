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
} // namespace Kaco