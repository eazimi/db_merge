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
#include "global_defines.hpp"
#include "commands.h"

using namespace std;

namespace Kaco
{
    class DbCompare
    {
        public:
            explicit DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2);
            explicit DbCompare(std::shared_ptr<IDbReader> local_db, std::shared_ptr<IDbReader> remote_db, std::shared_ptr<IDbReader> base_db);
            ~DbCompare();

            bool initialize();
            string compareAndMerge();
            void testDbDump();
            void testTableIndices();
            inline PA_VS2 diffTblNameDb() const { return m_table->diff_tname_db(); }
            PA_VS2 readDbTables() const;
            PA_MAP_S2 readDbTblSchema() const;
            inline VEC_TS3 diffSchemaDb() const { return m_table->diff_schema_db(); }
            inline PA_MAP_SVPS2 readDbTriggers() const { return m_trigger->read_trigger_db(); };
            inline T_MAP_SVPS2 triggers_db() const { return m_trigger->triggers_db(); };
            PA_VEC_PS2 readSingleTblTriggers(string table_name) const;
            inline T_VEC_PS2 trigger_tbl(string tbl_name) const { return m_trigger->trigger_tbl(tbl_name); }
            inline VEC_PS2 trigger_tbl(string tbl_name, DB_IDX db_idx) const { return m_trigger->trigger_tbl(tbl_name, db_idx); }
            PA_MAP_SVPS2 diffTriggerDb() const;
            PA_VEC_PS2 diffTriggerSingleTbl(string table_name) const;
            PA_VEC_PS2 diffTriggerSingleTbl(string table_name, DB_IDX db_idx1, DB_IDX db_idx2) const;
            VEC_PS2 updateTriggerSingleTbl(string table_name) const;
            inline string createTbl(string tbl_name) const { return m_table->create_tbl(tbl_name); }
            inline string insertInto(string tbl_name) const { return m_table->insert_into(tbl_name); }
            int attach_db(string remote_path, string base_path);
            vector<string> diff_records(string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2) const;
            inline string table_pk(string tbl_name, DB_IDX db_idx) const { return m_table->table_pk(tbl_name, db_idx); }
            T_VS3 records_status(string tbl_name) const;
            vector<string> table_cols(string tbl_name, DB_IDX db_idx) const;
        private:
            shared_ptr<IDbReader> m_db1, m_db2, m_base_db;
            shared_ptr<IDbReader> m_master_db;
            shared_ptr<Trigger> m_trigger;
            shared_ptr<Table> m_table;
            shared_ptr<Commands> m_commands;
            map<string, string> m_db1TblIndices, m_db2TblIndices;
            bool m_initialized;

            void initDbTableIndices();            
    };

} // namespace Kaco

#endif


