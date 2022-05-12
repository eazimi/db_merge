#ifndef TRIGGER_FUNCS_HPP
#define TRIGGER_FUNCS_HPP

#include <string>
#include <sstream>
#include <algorithm>
#include "data_types.hpp"

using namespace std;

namespace Kaco
{
    static auto formatString = [](string data)
    {
        stringstream ss;
        size_t input_len = data.length();
        for (auto i = 0; i < input_len; i++)
        {
            if ((data[i] != '\n') && (data[i] != ' '))
            {
                if (data[i] == '\"')
                    ss << "'"; 
                else ss << data[i];
            }
        }
        auto str_ss = ss.str();
        transform(str_ss.begin(), str_ss.end(), str_ss.begin(), ::tolower);
        return str_ss;
    };

    // TODO: create a class for any available object in db
    // data: vector<pair<trigger_name, trigger_sql>>>
    // returns vector<tuple<trigger_name, trigger_sql, formatted_trigger_sql>>
    static auto formatTriggers = [](const VEC_PS2 &data, vector<string> &vec_formatted_triggers)
    {
        vector<tuple<string, string, string>> formatted_triggers = {};
        for (auto vec : data)
        {
            auto str_formatted = formatString(vec.second); // format the trigger
            vec_formatted_triggers.push_back(str_formatted);
            formatted_triggers.push_back({vec.first, vec.second, str_formatted});
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

    static auto findTrigger = [](const VEC_PS2 &data, string str)
    {
        auto condition = [str](pair<string, string> p)
        {
            return (p.first == str);
        };
        auto it = find_if(data.begin(), data.end(), condition);
        return (it != end(data));
    };

    static auto updateNameInTriggerSql = [](string trigger_name, string trigger_sql, string new_trigger_name)
    {
        size_t pos_trigger_name = trigger_sql.find(trigger_name);
        if (pos_trigger_name != string::npos)
            trigger_sql.replace(pos_trigger_name, trigger_name.length(), new_trigger_name);
        return trigger_sql;
    };

} // namespace Kaco

#endif