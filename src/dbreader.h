#ifndef DBREADER_H
#define DBREADER_H

#include <sqlite3.h>
#include <string>
#include <functional>

using namespace std;

namespace Kaco
{
    typedef int (*ExecCallback)(void*, int, char**, char**);

    class DBReader
    {
        // TODO: set flags by a function
        // TODO: read path from a config file
        private:
            sqlite3 *db;
            string dbPath;

        public:
            explicit DBReader() = default;
            ~DBReader();

            bool connect(string dbPath);
            int sqlExec(string sql, ExecCallback cb);
            int dump_db(char *fileName);
            void run_db_command(string dbPath, string output, string command);
    };


} // namespace Kaco

#endif