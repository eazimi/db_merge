#include "trigger.h"
#include <sstream>
#include <algorithm>
#include <tuple>
#include <utility>
#include "global_funcs.hpp"
#include "trigger_funcs.hpp"
#include "global_defines.hpp"

namespace Kaco
{
    Trigger::Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db) : m_main_db(main_db), m_ref_db(ref_db)
    {
    }

    void Trigger::init_triggers(const vector<string> &main_tbls, const vector<string> &ref_tbls)
    {
        auto main_triggers = getDbTriggers(m_main_db, main_tbls);
        m_mainTriggers = std::move(main_triggers);

        auto ref_triggers = getDbTriggers(m_ref_db, ref_tbls);
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

    // returns triggers diff for a particular table
    // returns pair<vector<pair<trigger_name, trigger_sql>>, vector<pair<trigger_name, trigger_sql>>>
    // first: triggers which are in the main::table but not in the ref::table
    // second: triggers which are in the ref::table but not in the main::table
    PA_VEC_PS2 Trigger::diff_trigger_tbl(string tbl_name)
    {
        VEC_PS2 diff_triggers_main = {}, diff_triggers_ref = {}; // vector<pair<trigger_name, trigger_sql>>
        auto mainTblTriggers = m_mainTriggers[tbl_name];
        auto refTblTriggers = m_refTriggers[tbl_name];
        vector<string> vec_main_formatted_triggers = {}, vec_ref_formatted_triggers = {};
        // tuple<trigger_name, trigger_sql, formatted_trigger_sql>
        // get the formatted trigger in the second parameter of formatTriggers()
        VEC_TS3 mainTblTriggers_formatted = formatTriggers(mainTblTriggers, vec_main_formatted_triggers);
        VEC_TS3 refTblTriggers_formatted = formatTriggers(refTblTriggers, vec_ref_formatted_triggers);
        auto diff = diff_by_hash(vec_main_formatted_triggers, vec_ref_formatted_triggers);
        if (!diff.first.empty() && !mainTblTriggers_formatted.empty())
            diff_triggers_main = retrieveTriggers(mainTblTriggers_formatted, diff.first);
        if (!diff.second.empty() && !refTblTriggers_formatted.empty())
            diff_triggers_ref = retrieveTriggers(refTblTriggers_formatted, diff.second);
        return make_pair(diff_triggers_main, diff_triggers_ref);
    }

    // returns all the trigger diffs in the dbs
    // first: triggers in the main db but not in the ref db
    // second: triggers in the ref db but not in the main db
    // MAP_STR_VPS2: map<tbl_name, vector<pair<trigger_name, trigger_sql>>>
    PA_MAP_SVPS2 Trigger::diff_trigger_db(const vector<string> &main_tbls, const vector<string> &ref_tbls)
    {
        PA_MAP_SVPS2 diff_triggers = {};
        MAP_STR_VPS2 diff_triggers_main, diff_triggers_ref;
        auto common_tbls = getIntersect(main_tbls, ref_tbls);
        for (auto str : common_tbls)
        {
            auto main_trigger_vec = diff_trigger_tbl(str).first;
            if (!main_trigger_vec.empty())
                diff_triggers_main.insert({str, main_trigger_vec});

            auto ref_trigger_vec = diff_trigger_tbl(str).second;
            if (!ref_trigger_vec.empty())
                diff_triggers_ref.insert({str, ref_trigger_vec});
        }
        return {diff_triggers_main, diff_triggers_ref};
    }

    PA_VEC_PS2 Trigger::read_trigger_tbl(string tbl_name)
    {
        return make_pair(m_mainTriggers[tbl_name], m_refTriggers[tbl_name]);
    }

    // returns vector<pair<trigger_name, trigger_sql>>
    VEC_PS2 Trigger::update_trigger_tbl(string tbl_name)
    {
        VEC_PS2 updated_triggers = {};
        auto trigger_main = m_mainTriggers[tbl_name];
        auto trigger_ref = m_refTriggers[tbl_name];
        updated_triggers.assign(trigger_ref.begin(), trigger_ref.end());
        auto trigger_diff_main = diff_trigger_tbl(tbl_name).first;
        for (auto vec : trigger_diff_main)
        {
            if(vec.second.empty())
                continue;
            // TODO: do the following check according the values that are read from a config file
            bool keep_trigger = true;
            if (keep_trigger)
            {
                bool trigger_name_found = findTrigger(trigger_ref, vec.first);
                if (trigger_name_found)
                {
                    auto new_trigger_name = vec.first;
                    new_trigger_name.append("_main");
                    auto new_tigger_sql = updateNameInTriggerSql(vec.first, vec.second, new_trigger_name);
                    updated_triggers.push_back(make_pair(new_trigger_name, new_tigger_sql));
                    continue;   
                }
                updated_triggers.push_back(vec);
            }
        }
        return updated_triggers;
    }

} // namespace Kaco