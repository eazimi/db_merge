#ifndef GLOBAL_FUNCS_HPP
#define GLOBAL_FUNCS_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <utility>
#include <tuple>
#include "data_types.hpp"

using namespace std;

namespace Kaco
{
    static pair<vector<string>, vector<string>> getDiff(vector<string> main, vector<string> ref)
    {
        vector<string> main_diff = {};
        vector<string> ref_diff = {};
        sort(main.begin(), main.end());
        sort(ref.begin(), ref.end());
        set_difference(main.begin(), main.end(), ref.begin(), ref.end(), back_inserter(main_diff));
        set_difference(ref.begin(), ref.end(), main.begin(), main.end(), back_inserter(ref_diff));
        return {main_diff, ref_diff};
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

    static void print(vector<pair<string, string>> data,
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
} // namespace Kaco

#endif