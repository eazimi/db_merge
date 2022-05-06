#ifndef DBCOMPARE_H
#define DBCOMPARE_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include "trigger.h"
#include "table.h"
#include "IDbReader.hpp"
#include "data_types.hpp"

using namespace std;

namespace Kaco
{
    class DbCompare
    {
        public:
            explicit DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2);
            ~DbCompare();

            bool initialize();
            string compareAndMerge();
            void testDbDump();
            void testTableIndices();
            void testCreateNewTbl();
            inline PA_VS2 diffTblNameDb() const { return m_table->diffTblNameDb(); }
            PA_VS2 readDbTables() const;
            PA_MAP_S2 readDbTblSchema() const;
            inline VEC_TS3 diffSchemaDb() const { return m_table->diffSchemaDb(); }
            inline PA_MAP_SVPS2 readDbTriggers() const { return m_trigger->readDbTriggers(); };
            PA_VEC_PS2 readSingleTblTriggers(string table_name) const;
            inline PA_MAP_SVPS2 diffTriggerDb() const { return m_trigger->diffTriggerDb(m_mainTbls, m_refTbls); }
            PA_VEC_PS2 diffTriggerSingleTbl(string table_name) const;
            VEC_PS2 updateTriggerSingleTbl(string table_name) const;

        private:
            shared_ptr<IDbReader> m_db1, m_db2;
            shared_ptr<Trigger> m_trigger;
            shared_ptr<Table> m_table;
            vector<string> m_mainTbls, m_refTbls;
            map<string, string> m_db1TblIndices, m_db2TblIndices;
            bool m_initialized;

            void initDbTables();
            void initDbTableIndices();
            string createNewTbl(string tblName);
    };

} // namespace Kaco

#endif


