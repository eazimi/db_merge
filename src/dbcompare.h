#ifndef DBCOMPARE_H
#define DBCOMPARE_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include "IDbReader.hpp"

namespace Kaco
{
    class DbCompare
    {
        public:
            DbCompare();
            DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2);
            ~DbCompare();

            // std::vector<std::string> compareSchema(std::string tableName);
            bool initialize();
            std::string compareAndMerge();
            void testDbDump();
            void testGetTables();
            void testTableSchema();
            void testTableIndices();
            void testTableTriggers();
            void testTableTriggers(std::string tableName);
            void testCreateNewTbl();
            void testDiffTableNames();
            void testDiffTableSchemas();
            void testDiffTableTriggers();

        private:
            std::shared_ptr<IDbReader> m_db1, m_db2;
            std::vector<std::string> m_mainTbls, m_refTbls;
            std::map<std::string, std::string> m_mainTblSchema, m_refTblSchema;
            std::map<std::string, std::string> m_db1TblIndices, m_db2TblIndices;
            std::map<std::string, std::string> m_mainTblTriggers, m_refTblTriggers;
            bool m_initialized;

            void initDbTables();
            void initDbTableSchema();
            void initDbTableIndices();
            void initDbTableTriggers();
            std::string createNewTbl(std::string tblName);
            pair<vector<string>, vector<string>> diffTblNames();
            vector<tuple<string, string, string>> diffTblSchemas(); // 0: table name, 1: table schema in main db, 2: table schema in ref db
            map<string, string> diffTblTriggers(); // returns pair<tbl_name, ref_triggers>
    };

} // namespace Kaco

#endif


