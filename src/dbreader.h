#ifndef DBREADER_H
#define DBREADER_H

#include <sqlite3.h>
#include <string>

using namespace std;

namespace Kaco
{
    class DBReader
    {
        // TODO: set flags by a function
        // TODO: read path from a config file
        private:
            sqlite3 *db;

        public:
            explicit DBReader() = default;
            ~DBReader();

            bool connect(string dbPath);
    };


} // namespace Kaco

#endif