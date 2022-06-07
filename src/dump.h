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
    private:
        sqlite3_stmt *stmt_table = nullptr;
        sqlite3_stmt *stmt_data = nullptr;
        ostringstream oss;
        void save_dump(string dump_path);
        void cleanup();
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
        
        friend class DumpBuilder;
        friend class DumpTableBuilder;
        friend class DumpTriggerBuilder;
    };
} // namespace Kaco

#endif
