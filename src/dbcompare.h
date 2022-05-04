#ifndef DBCOMPARE_H
#define DBCOMPARE_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include "IDbReader.hpp"
#include "data_types.hpp"

using namespace std;

namespace Kaco
{
    class DbCompare
    {
        public:
            DbCompare();
            DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2);
            ~DbCompare();

            bool initialize();
            string compareAndMerge();
            void testDbDump();
            void testGetTables();
            void testTableSchema();
            void testTableIndices();
            void testTableTriggers();
            void testTableTriggers2();
            void testTableTriggers(string tableName);
            void testTableTriggers2(string tableName);
            void testCreateNewTbl();
            void testDiffTableNames();
            void testDiffTableSchemas();
            void testDiffTableTriggers();
            void testDiffTableTriggers2();
            void testDiffTableTriggers2(string table_name);

        private:
            shared_ptr<IDbReader> m_db1, m_db2;
            vector<string> m_mainTbls, m_refTbls;
            map<string, string> m_mainTblSchema, m_refTblSchema;
            map<string, string> m_db1TblIndices, m_db2TblIndices;
            map<string, string> m_mainTblTriggers, m_refTblTriggers;
            MAP_STR_VPS2 m_mainTblTriggers2, m_refTblTriggers2;
            bool m_initialized;

            void initDbTables();
            void initDbTableSchema();
            void initDbTableIndices();
            void initDbTableTriggers();
            void initDbTableTriggers2();
            string createNewTbl(string tblName);
            pair<vector<string>, vector<string>> diffTblNames();
            vector<tuple<string, string, string>> diffTblSchemas(); // 0: table name, 1: table schema in main db, 2: table schema in ref db
            map<string, string> diffTblTriggers(); // returns pair<tbl_name, ref_triggers>
            PA_MAP_SVPS2 diffTblTriggers2(); // returns diff in triggers
            PA_VEC_PS2 diffTblTriggers2(string table_name); // returns triggers diff for a particular table 
    };

} // namespace Kaco

#endif


