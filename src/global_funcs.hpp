#ifndef GLOBAL_FUNCS_HPP
#define GLOBAL_FUNCS_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <utility>
#include <tuple>
#include <functional>
#include <iomanip>
#include "data_types.hpp"

using namespace std;

namespace Kaco
{
    static PA_VS2 getDiff(vector<string> main, vector<string> ref)
    {
        vector<string> main_diff = {};
        vector<string> ref_diff = {};
        sort(main.begin(), main.end());
        sort(ref.begin(), ref.end());
        set_difference(main.begin(), main.end(), ref.begin(), ref.end(), back_inserter(main_diff));
        set_difference(ref.begin(), ref.end(), main.begin(), main.end(), back_inserter(ref_diff));
        return make_pair(main_diff, ref_diff);
    }

    // assumes the input data elements are unique
    static vector<size_t> get_hash(vector<string> data, map<size_t, string> &hash_map)
    {
        hash<string> hash_func;
        vector<size_t> hash_data;
        for (auto str : data)
        {
            if(str.empty())
                continue;
            auto hash_vec = hash_func(str);
            hash_data.push_back(hash_vec);
            hash_map.insert({hash_vec, str});
        }
        return hash_data;
    }

    static vector<string> hash_2_str(vector<size_t> data, map<size_t, string> hash_tbl)
    {
        vector<string> strings;
        for (auto i : data)
        {
            string value = hash_tbl[i];
            if (!value.empty())
                strings.push_back(value);
        }
        return strings;
    }

    static PA_VS2 diff_by_hash(vector<string> main, vector<string> ref)
    {
        vector<size_t> hash_main = {}, hash_ref = {};
        map<size_t, string> map_main, map_ref;
        hash_main = std::move(get_hash(main, map_main));
        hash_ref = std::move(get_hash(ref, map_ref));

        vector<size_t> diff_hash_main = {};
        vector<size_t> diff_hash_ref = {};
        sort(hash_main.begin(), hash_main.end());
        sort(hash_ref.begin(), hash_ref.end());
        set_difference(hash_main.begin(), hash_main.end(), hash_ref.begin(), hash_ref.end(), back_inserter(diff_hash_main));
        set_difference(hash_ref.begin(), hash_ref.end(), hash_main.begin(), hash_main.end(), back_inserter(diff_hash_ref));

        vector<string> diff_main = {}, diff_ref = {};
        diff_main = hash_2_str(diff_hash_main, map_main);
        diff_ref = hash_2_str(diff_hash_ref, map_ref);
        return make_pair(diff_main, diff_ref);
    }

    static vector<string> getIntersect(vector<string> main, vector<string> ref)
    {
        vector<string> intersect = {};
        sort(main.begin(), main.end());
        sort(ref.begin(), ref.end());
        set_intersection(main.begin(), main.end(), ref.begin(), ref.end(),
                         std::back_inserter(intersect));
        sort(intersect.begin(), intersect.end());
        return intersect;
    }

    template <typename T>
    static void print(string message, T data)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (const auto &str : data)
            cout << str << endl;
    }

    static void print(VEC_TS3 data, string main_msg, string aux_msg1, string aux_msg2)
    {
        cout << endl
             << "\"" << main_msg << "\"" << endl;
        for (const auto &str : data)
            cout << "'[" << get<0>(str) << "]'" << endl
                 << "'# " << aux_msg1 << "'" << endl 
                 << get<1>(str) << endl
                 << "'# " << aux_msg2 << "'" << endl
                 << get<2>(str) << endl;
    }

    static void print(string main_msg, string aux_msg, map<string, string> data)
    {
        cout << endl
             << "\"" << main_msg << "\"" << endl;
        for (const auto &str : data)
            cout << "'# " << aux_msg << " [" << get<0>(str) << "]'" << endl
                 << get<1>(str) << endl;
    }

    static void print(map<string, vector<pair<string, string>>> data,
                      string message,
                      string schema,
                      bool print_sql = true)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (auto const &rec : data)
        {
            auto tbl_name = rec.first;
            auto vec_triggers = rec.second;
            for (auto const &trigger : vec_triggers)
            {
                auto trigger_name = trigger.first;
                auto trigger_sql = trigger.second;
                if(print_sql)
                    cout << endl
                         << "'[" << schema << "::" << tbl_name
                         << "::" << trigger_name << "]'" << endl
                         << trigger_sql << endl;
                else
                    cout << "'[" << schema << "::" << tbl_name
                         << "::" << trigger_name << "]'" << endl;
            }
        }
    }

    static void print(VEC_PS2 data,
                      string message,
                      string schema,
                      string tbl_name,
                      bool print_sql = true)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (auto const &vec : data)
        {
            auto trigger_name = vec.first;
            auto trigger_sql = vec.second;
            if (print_sql)
                cout << endl
                     << "'[" << schema << "::" << tbl_name
                     << "::" << trigger_name << "]'" << endl
                     << trigger_sql << endl;
            else
                cout << "'[" << schema << "::" << tbl_name
                     << "::" << trigger_name << "]'" << endl;
        }
    }

    static void print(map<string, string> data, string message)
    {
        cout << endl
             << "\"" << message << "\"" << endl;
        for (const auto &str : data)
            cout << str.first << ": " << str.second << endl;
    }

    static auto cb_sql_exec = [](void *buffer, int cnt, char **row, char **cols)
    {
        stringstream ss_data;
        for (auto i = 0; i < cnt; i++)
        {
            if (i)
                ss_data << VAL_SEPERATOR;
            if (row[i])
                ss_data << row[i];
        }
        auto ptr_buffer = static_cast<vector<string> *>(buffer);
        ptr_buffer->push_back(ss_data.str());
        return 0;
    };
    
} // namespace Kaco

#endif