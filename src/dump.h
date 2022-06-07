#ifndef DUMP_H
#define DUMP_H

#include <sqlite3.h>

namespace Kaco
{
    class DumpBuilder;

    class Dump
    {
    private:
        sqlite3_stmt *stmt_table = nullptr;
        sqlite3_stmt *stmt_data = nullptr;
        Dump() {}

    public:
        ~Dump() {}
        static DumpBuilder create();
        
        friend class DumpBuilder;
        friend class DumpTableBuilder;
        friend class DumpTriggerBuilder;
    };
} // namespace Kaco

#endif
