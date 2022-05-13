#include "trigger.h"
#include <sstream>
#include <algorithm>
#include <tuple>
#include <utility>
#include "global_funcs.hpp"
#include "trigger_funcs.hpp"

namespace Kaco
{
    Trigger::Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db) : m_main_db(main_db), m_ref_db(ref_db)
    {
        for (auto i = 0; i < DB_CNT; i++)
            m_triggers[i] = {};
    }

    Trigger::Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db, shared_ptr<IDbReader> base_db)
    {
        m_main_db = main_db;
        m_ref_db = ref_db;
        m_db[DB_IDX::local] = main_db;
        m_db[DB_IDX::remote] = ref_db;
        m_db[DB_IDX::base] = base_db;
        for (auto i = 0; i < DB_CNT; i++)
            m_triggers[i] = {};
    }

    void Trigger::init_triggers(const vector<string> &main_tbls, const vector<string> &ref_tbls)
    {
        auto main_triggers = getDbTriggers(m_main_db, main_tbls);
        m_mainTriggers = std::move(main_triggers);

        auto ref_triggers = getDbTriggers(m_ref_db, ref_tbls);
        m_refTriggers = std::move(ref_triggers);
    }

    void Trigger::init_triggers(const vector<string> &main_tbls, const vector<string> &ref_tbls, const vector<string> &base_tbls)
    {
        auto main_triggers = getDbTriggers(m_db[DB_IDX::local], main_tbls);
        m_triggers[DB_IDX::local] = std::move(main_triggers);

        auto ref_triggers = getDbTriggers(m_db[DB_IDX::remote], ref_tbls);
        m_triggers[DB_IDX::remote] = std::move(ref_triggers);

        auto base_triggers = getDbTriggers(m_db[DB_IDX::base], base_tbls);
        m_triggers[DB_IDX::base] = std::move(base_triggers);
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
        // auto diff = getDiff(vec_main_formatted_triggers, vec_ref_formatted_triggers);
        auto diff = diff_by_hash(vec_main_formatted_triggers, vec_ref_formatted_triggers);
        if (!diff.first.empty() && !mainTblTriggers_formatted.empty())
            diff_triggers_main = retrieveTriggers(mainTblTriggers_formatted, diff.first);
        if (!diff.second.empty() && !refTblTriggers_formatted.empty())
            diff_triggers_ref = retrieveTriggers(refTblTriggers_formatted, diff.second);
        return make_pair(diff_triggers_main, diff_triggers_ref);
    }

    // returns triggers diff for a particular table in two particular dbs
    // returns pair<vector<pair<trigger_name, trigger_sql>>, vector<pair<trigger_name, trigger_sql>>>
    // first: triggers which are in the main::table but not in the ref::table
    // second: triggers which are in the ref::table but not in the main::table
    PA_VEC_PS2 Trigger::diff_trigger_tbl(string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2)
    {
        VEC_PS2 diff_main = {}, diff_ref = {}; // vector<pair<trigger_name, trigger_sql>>
        
        auto main_triggers = m_triggers[db_idx1][tbl_name];
        auto ref_triggers = m_triggers[db_idx2][tbl_name];

        vector<string> triggers_main_plain = {}, triggers_ref_plain = {};
        // tuple<trigger_name, trigger_sql, formatted_trigger_sql>
        // get the formatted trigger in the second parameter of formatTriggers()
        VEC_TS3 triggers_main_fmt = formatTriggers(main_triggers, triggers_main_plain);
        VEC_TS3 triggers_ref_fmt = formatTriggers(ref_triggers, triggers_ref_plain);
        // auto diff = getDiff(vec_main_formatted_triggers, vec_ref_formatted_triggers);
        auto diff = diff_by_hash(triggers_main_plain, triggers_ref_plain);
        if (!diff.first.empty() && !triggers_main_fmt.empty())
            diff_main = retrieveTriggers(triggers_main_fmt, diff.first);
        if (!diff.second.empty() && !triggers_ref_fmt.empty())
            diff_ref = retrieveTriggers(triggers_ref_fmt, diff.second);
        return make_pair(diff_main, diff_ref);
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

    T_MAP_SVPS2 Trigger::triggers_db() const
    {
        return make_tuple(m_triggers[DB_IDX::local], m_triggers[DB_IDX::remote], m_triggers[DB_IDX::base]);
    }

    PA_VEC_PS2 Trigger::read_trigger_tbl(string tbl_name)
    {
        return make_pair(m_mainTriggers[tbl_name], m_refTriggers[tbl_name]);
    }

    T_VEC_PS2 Trigger::trigger_tbl(string tbl_name)
    {
        auto trigger_local = m_triggers[DB_IDX::local][tbl_name];
        auto trigger_remote = m_triggers[DB_IDX::remote][tbl_name];
        auto trigger_base = m_triggers[DB_IDX::base][tbl_name];
        return make_tuple(std::move(trigger_local), std::move(trigger_remote), std::move(trigger_base));
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