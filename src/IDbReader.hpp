#ifndef IDBREADER_HPP
#define IDBREADER_HPP

#include <sqlite3.h>
#include <string>
#include <vector>

using namespace std;

namespace Kaco
{
    class IDbReader
    {
    public:
        virtual bool connect(string dbPath) = 0;
        virtual int attach_db(string dbPath) = 0;
        virtual int dbDump(char *fileName) = 0;
        virtual vector<string> getTables() = 0;
        virtual vector<string> getTableSchema(string tableName) = 0;
        virtual vector<string> getIndices(string tableName) = 0;
        virtual vector<pair<string, string>> getTriggers(string tableName) = 0;
        virtual string getCreateTblCmd(string tblName) = 0;
    };

} // namespace Kaco

#endif