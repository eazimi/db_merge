#include "trigger.h"
#include <sstream>
#include <algorithm>
#include <tuple>
#include <utility>
#include "global_funcs.hpp"

namespace Kaco
{
    static auto formatString = [](string data)
    {
        stringstream ss;
        size_t input_len = data.length();
        for (auto i = 0; i < input_len; i++)
        {
            if ((data[i] != '\n') && (data[i] != ' '))
                ss << data[i];
        }
        return ss.str();
    };

    // TODO: create a class for any available object in db
    // data: vector<pair<trigger_name, trigger_sql>>>
    // returns vector<tuple<trigger_name, trigger_sql, formatted_trigger_sql>>
    static auto formatTriggers = [](const vector<pair<string, string>> &data, vector<string> &vec_formatted_triggers)
    {
        vector<tuple<string, string, string>> formatted_triggers = {};
        for (auto vec : data)
        {
            auto formatted = formatString(vec.second); // format the trigger
            vec_formatted_triggers.push_back(formatted);
            formatted_triggers.push_back({vec.first, vec.second, formatted});
        }
        return formatted_triggers;
    };

    // tuple<trigger_name, trigger_sql, formatted_trigger_sql>
    // retruns vector<pair<trigger_name, trigger_sql>>
    static auto retrieveTriggers = [](vector<tuple<string, string, string>> data,
                                      vector<string> vec_formatted_triggers)
    {
        auto sortBy3rd = [](const tuple<string, string, string> &a, tuple<string, string, string> b)
        {
            bool comparison = get<2>(a) < get<2>(b);
            return comparison;
        };
        vector<pair<string, string>> triggers;
        sort(vec_formatted_triggers.begin(), vec_formatted_triggers.end());
        sort(data.begin(), data.end(), sortBy3rd);
        auto j = 0;
        int data_size = data.size();
        for (auto i = 0; i < data_size; i++)
        {
            if (get<2>(data[i]) == vec_formatted_triggers[j])
            {
                triggers.push_back(make_pair(get<0>(data[i]), get<1>(data[i])));
                j++;
            }
        }
        return triggers;
    };

    Trigger::Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db) : m_main_db(main_db), m_ref_db(ref_db)
    {
    }

    void Trigger::initDbTriggers(const vector<string> &main_tbls, const vector<string> &ref_tbls)
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
    PA_VEC_PS2 Trigger::diffTriggerSingleTbl(string tbl_name)
    {
        VEC_PS2 diff_triggers_main = {}, diff_triggers_ref = {}; // vector<pair<trigger_name, trigger_sql>>

        auto mainTblTriggers = m_mainTriggers[tbl_name];
        auto refTblTriggers = m_refTriggers[tbl_name];
        vector<string> vec_main_formatted_triggers = {}, vec_ref_formatted_triggers = {};

        // tuple<trigger_name, trigger_sql, formatted_trigger_sql>
        // get the formatted trigger in the second parameter of formatTriggers()
        VEC_TS3 mainTblTriggers_formatted = formatTriggers(mainTblTriggers, vec_main_formatted_triggers);
        VEC_TS3 refTblTriggers_formatted = formatTriggers(refTblTriggers, vec_ref_formatted_triggers);

        auto diff = getDiff(vec_main_formatted_triggers, vec_ref_formatted_triggers);

        if (!diff.first.empty() && !mainTblTriggers_formatted.empty())
            diff_triggers_main = retrieveTriggers(mainTblTriggers_formatted, diff.first);
        if (!diff.second.empty() && !refTblTriggers_formatted.empty())
            diff_triggers_ref = retrieveTriggers(refTblTriggers_formatted, diff.second);

        return make_pair(diff_triggers_main, diff_triggers_ref);
    }

} // namespace Kaco