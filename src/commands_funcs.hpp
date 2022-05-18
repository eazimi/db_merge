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
    
} // namespace

#endif