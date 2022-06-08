#ifndef DUMP_H
#define DUMP_H

#include <sqlite3.h>
#include <string>
#include <sstream>

using namespace std;

namespace Kaco
{
    class DumpBuilder;

    class Dump
    {
        sqlite3_stmt *stmt_table = nullptr;
        sqlite3_stmt *stmt_data = nullptr;
        ostringstream oss;        
        void cleanup();
        bool prepare(sqlite3 *db, const string &cmd, sqlite3_stmt *stmt);
        bool table_info(string &tbl_data, string &tbl_name);
        string table_record();
        string table_data(string tbl_name);
        string db_triggers();
        Dump() {}

    public:
        ~Dump() {}
        
        Dump(Dump &&other) : oss(move(other.oss))
        {
        }
        
        Dump &operator=(Dump &&other)
        {
            if(this == &other)
                return *this;
            oss = move(other.oss);
            return *this;            
        }

        static DumpBuilder create();
        void save_dump(string dump_path);
        
        friend class DumpBuilder;
        friend class DumpTableBuilder;
        friend class DumpTriggerBuilder;
    };
} // namespace Kaco

#endif
