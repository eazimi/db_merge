#ifndef COMMANDS_FUNCS_HPP
#define COMMANDS_FUNCS_HPP

#include <algorithm>
#include <string>
#include <map>
#include <utility>
#include <cstring>
#include "global_defines.hpp"

using namespace std;

namespace Kaco
{
    static string remove_char(string str_input, char ch)
    {
        auto it = remove(str_input.begin(), str_input.end(), ch);
        str_input.erase(it, str_input.end());
        return str_input;
    }

    static map<string, string> format_recs(const vector<string> &records, vector<string> &formatted_recs)
    {
        map<string, string> map_recs = {};
        for (auto rec : records)
        {
            auto formatted = remove_char(rec, '|');            
            map_recs.insert(make_pair(formatted, rec));
            formatted_recs.push_back(formatted);
        }
        return map_recs;
    }

    static vector<string> retrieve_records(const vector<string> &records, const map<string, string> &map_recs)
    {
        vector<string> unformatted_recs = {};
        for(auto rec: records)
        {
            auto it = map_recs.find(rec);
            if(it != map_recs.end())
                unformatted_recs.push_back(it->second);
        }
        return unformatted_recs;
    }

    static VEC_PS2 match_col_val(string record, const vector<string> &cols)
    {
        VEC_PS2 col_val = {};
        int cols_size = cols.size();
        int start_pos = 0;
        int i = 0;
        while (true)
        {
            int pos = record.find_first_of(VAL_SEPERATOR, start_pos);
            auto val = record.substr(start_pos, pos - start_pos);
            col_val.push_back(std::move(make_pair(cols[i], val)));
            if (++i >= cols_size)
                break;
            start_pos = pos + 1;
        }
        return col_val;
    }

    static string pk_value(string record)
    {
        int pos = record.find_first_of(VAL_SEPERATOR);
        auto value = record.substr(0, pos);
        return value;
    }

    static map<string, string> map_col_record(const vector<string> &records)
    {
        map<string, string> map_cr = {};
        for(auto record:records)
        {
            int pos = record.find_first_of(VAL_SEPERATOR);
            auto col = record.substr(0, pos);
            map_cr.insert(make_pair(col, record));
        }
        return map_cr;
    }

    static string col_equal_val(VEC_PS2 col_val, bool skip_empty = false)
    {
        stringstream ss;
        for (auto cv : col_val)
        {
            if (skip_empty && cv.second.empty())
                continue;
            ss << cv.first << "="
               << "'" << cv.second << "'"
               << " AND ";
        }
        auto ss_str = ss.str();
        int pos_and = ss_str.find_last_of("AND");
        ss_str.replace(pos_and - 3, 5, "");
        ss_str.append(";");
        return ss_str;
    }

    static pair<string, string> col_val_par(VEC_PS2 col_val, bool skip_empty = false)
    {
        stringstream ss_col;
        stringstream ss_val;
        ss_col << "(";
        ss_val << "(";
        for (auto cv : col_val)
        {
            if(skip_empty && cv.second.empty())
                continue;
            ss_col << cv.first << ", ";
            ss_val << "'" << cv.second << "'" << ", ";
        }
        ss_col << ")";
        ss_val << ");";
        auto str_ss_col = ss_col.str();
        auto str_ss_val = ss_val.str();
        auto pos_col = str_ss_col.find_last_of(",");
        auto pos_val = str_ss_val.find_last_of(",");
        str_ss_col.replace(pos_col, 2, "");
        str_ss_val.replace(pos_val, 2, "");
        return make_pair(str_ss_col, str_ss_val);
    }

} // namespace

#endif