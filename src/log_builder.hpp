#ifndef MAG_BUILDER_HPP
#define MAG_BUILDER_HPP

#include "log.h"
#include <string>
#include <utility>
#include <sstream>

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

        Self &add_table(string schema, string tbl_name)
        {
            msg.schema = schema;
            msg.tbl_name = tbl_name;
            return *this;
        }

        Self &add_msg_text(string msg_text)
        {
            msg.msg_text = msg_text;
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

        Self &add_records(string caption, vector<string> records)
        {
            msg.captions.emplace_back(caption);
            msg.records.emplace_back(records);
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
    };
} // namespace Kaco

#endif