#ifndef DBREADER_H
#define DBREADER_H

#include <string>
#include <functional>
#include <vector>
#include <utility>
#include "IDBReader.hpp"

using namespace std;

namespace Kaco
{
    typedef int (*ExecCallback)(void*, int, char**, char**);

    class DbReader : public IDbReader
    {
        // TODO: set flags by a function
        // TODO: read path from a config file
        // TODO: refactor dbDump()
        public:
            explicit DbReader() = default;
            ~DbReader();

            bool connect(string dbPath);
            int sql_exec(string sql, ExecCallback cb);
            vector<string> sql_exec(string cmd);
            int dbDump(char *fileName);
            void command_exec(string dbPath, string output, string command);
            void dbDiff(string dbPath1, string dbPath2, string output);
            vector<string> getTables();
            vector<string> getTableSchema(string tableName);
            string getCreateTblCmd(string tblName);
            vector<pair<string, string>> getTriggers(string tableName);
            vector<string> getIndices(string tableName);

        private:
            sqlite3 *db;
    };

} // namespace Kaco

#endif