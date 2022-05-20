#ifndef TRIGGER_H
#define TRIGGER_H

#include <memory>
#include <utility>
#include <array>
#include "data_types.hpp"
#include "IDbReader.hpp"
#include "global_defines.hpp"

using namespace std;
namespace Kaco
{
    class Trigger
    {   
        private:
            IDbReader *m_main_db, *m_ref_db, *m_base_db;
            array<IDbReader *, DB_CNT> m_db;
            MAP_STR_VPS2 m_mainTriggers, m_refTriggers;
            array<MAP_STR_VPS2, DB_CNT> m_triggers;
            MAP_STR_VPS2 getDbTriggers(IDbReader *db, const vector<string> &db_tbls);

        public:
            Trigger(IDbReader *main_db, IDbReader *ref_db);
            Trigger(IDbReader *main_db, IDbReader *ref_db, IDbReader *base_db);
            void init_triggers(const vector<string> &main_tbls, const vector<string> &ref_tbls);
            void init_triggers(const vector<string> &main_tbls, const vector<string> &ref_tbls, const vector<string> &base_tbls);
            PA_VEC_PS2 diff_trigger_tbl(string tbl_name); // returns triggers diff for a particular table
            PA_VEC_PS2 diff_trigger_tbl(string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2); // returns triggers diff for a particular table in two particular dbs
            PA_MAP_SVPS2 diff_trigger_db(const vector<string> &main_tbls, const vector<string> &ref_tbls); // returns all the trigger diffs in the dbs
            inline PA_MAP_SVPS2 read_trigger_db() const { return make_pair(m_mainTriggers, m_refTriggers); }
            T_MAP_SVPS2 triggers_db() const;
            PA_VEC_PS2 read_trigger_tbl(string tbl_name);
            T_VEC_PS2 trigger_tbl(string tbl_name);
            inline VEC_PS2 trigger_tbl(string tbl_name, DB_IDX db_idx) { return m_triggers[db_idx][tbl_name]; }
            VEC_PS2 update_trigger_tbl(string tbl_name);
    };
} // namespace Kaco

#endif