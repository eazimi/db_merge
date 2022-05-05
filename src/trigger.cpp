#include "trigger.h"
#include "dbcompare_funcs.hpp"
namespace Kaco
{
    Trigger::Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db) : m_main_db(main_db), m_ref_db(ref_db)
    {
    }

    void Trigger::initDbTriggers(const vector<string> &main_tbls, const vector<string> &ref_tbls)
    {
        auto main_triggers = getDbTriggers(m_main_db, main_tbls);
        m_mainTriggers = std::move(main_triggers);

        auto ref_triggers = initTblTriggers(m_ref_db, ref_tbls);
        m_refTriggers = std::move(ref_triggers);
    }

    MAP_STR_VPS2 Trigger::getDbTriggers(const shared_ptr<IDbReader> &db, const vector<string> &db_tbls)
    {
        MAP_STR_VPS2 tbl_triggers = {};
        for (auto tbl : db_tbls)
        {
            auto triggers = db->getTriggers(tbl);
            auto pair_tbl_triggers = make_pair(tbl, triggers);
            tbl_triggers.insert(std::move(pair_tbl_triggers));
        }
        return tbl_triggers;
    }

} // namespace Kaco