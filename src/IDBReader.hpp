#ifndef IDBREADER_HPP
#define IDBREADER_HPP

#include <sqlite3.h>
#include <string>
#include <vector>

namespace Kaco
{
    class IDbReader
    {
    public:
        virtual bool connect(std::string dbPath) = 0;
        virtual std::vector<std::string> getTables() = 0;
        virtual std::vector<std::string> getTableSchema(std::string tableName) = 0;
        virtual std::vector<std::string> getIndices(std::string tableName) = 0;
        virtual std::vector<std::pair<std::string, std::string>> getTriggers(std::string tableName) = 0;
        virtual std::string getCreateTblSQL(std::string tblName) = 0;
    };

} // namespace Kaco

#endif