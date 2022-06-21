#ifndef LOG_BUILDER_HPP
#define LOG_BUILDER_HPP

#include "log.h"
#include <string>
#include <utility>
#include <sstream>
#include <tuple>
#include <iostream>

using namespace std;

namespace Kaco
{
    class LogBuilder
    {
        using Self = LogBuilder;

    private:
        Log msg;

    public:
        LogBuilder(int indent)
        {
            msg.indent = indent;
        }

        Self &add_table(string tbl_name)
        {
            msg.tbl_name = tbl_name;
            return *this;
        }

        Self &add_schema(string schema)
        {
            msg.schema = schema;
            return *this;
        }

        Self &add_schema_aux(string schema_aux)
        {
            msg.schema_aux = schema_aux;
            return *this;
        }

        Self &add_msg_text(string msg_text)
        {
            msg.msg_text = msg_text;
            return *this;
        }

        Self &add_msg_multi(string msg_text)
        {
            msg.msg_multi.emplace_back(msg_text);
            return *this;
        }

        Self &add_msg_multi(vector<string> msg_text)
        {
            msg.msg_multi = move(msg_text);
            return *this;
        }

        Self &add_col_names(vector<string> col_names)
        {
            ostringstream oss;
            int col_names_size = col_names.size();
            for (auto i = 0; i < col_names_size; i++)
            {
                oss << col_names[i];
                if (i < col_names_size - 1)
                    oss << "|";
            }
            msg.col_names = oss.str();
            return *this;
        }

        Self &add_col_names(string col_names)
        {
            msg.col_names = col_names;
            return *this;
        }

        Self &add_data(string caption, vector<string> data)
        {
            msg.captions.emplace_back(caption);
            msg.data.emplace_back(data);
            return *this;
        }

        Self &add_data(vector<string> caption, vector<tuple<string, string, string>> data)
        {
            msg.captions = move(caption);
            msg.vec_tu_data = move(data);
            return *this;
        }

        Self &add_captions(vector<string> captions)
        {
            msg.captions = move(captions);
            return *this;
        }

        Self &add_data(vector<vector<pair<string, string>>> data)
        {
            msg.vec_pa_data = move(data);
            return *this;
        }

        Self &set_indent(int indent)
        {
            msg.indent = indent;
            return *this;
        }

        operator Log() const
        {
            return msg;
        }

        inline string str_records() { return msg.str_records(); }
        inline string str_no_pk() { return msg.str_no_pk(); }
        inline string str_diff_tbls() { return msg.str_diff_tbls(); }
        inline string str_schema() { return msg.str_schema(); }
    };
} // namespace Kaco

#endif