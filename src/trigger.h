#ifndef TRIGGER_H
#define TRIGGER_H

#include <memory>
#include <utility>
#include "data_types.hpp"
#include "IDbReader.hpp"

using namespace std;
namespace Kaco
{
    class Trigger
    {   
        private:
            shared_ptr<IDbReader> m_main_db, m_ref_db;
            MAP_STR_VPS2 m_mainTriggers, m_refTriggers;
            MAP_STR_VPS2 getDbTriggers(const shared_ptr<IDbReader> &db, const vector<string> &db_tbls);

        public:
            Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db);
            void initDbTriggers(const vector<string> &main_tbls, const vector<string> &ref_tbls);
            PA_VEC_PS2 diffTriggerSingleTbl(string tbl_name); // returns triggers diff for a particular table
            PA_MAP_SVPS2 diffTriggerDb(const vector<string> &main_tbls, const vector<string> &ref_tbls); // returns all the trigger diffs in the dbs
            inline PA_MAP_SVPS2 readDbTriggers() const { return make_pair(m_mainTriggers, m_refTriggers); }
            PA_VEC_PS2 readSingleTblTriggers(string tbl_name);
            VEC_PS2 updateTriggerSingleTbl(string tbl_name);
    };
} // namespace Kaco

#endif