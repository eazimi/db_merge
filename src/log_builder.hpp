#ifndef MAG_BUILDER_HPP
#define MAG_BUILDER_HPP

#include "log.h"
#include <string>
#include <utility>

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

        Self &add_tbl(string schema, string tbl_name)
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

        Self &add_col_names(string col_names)
        {
            msg.col_names = col_names;
            return *this;
        }

        Self &add_caption(string caption_text, string caption_value)
        {
            msg.captions.emplace_back(make_pair(caption_text, caption_value));
            return *this;
        }

        operator Log() const
        {
            return msg;
        }

        inline string str() { return msg.str(); }
    };
} // namespace Kaco

#endif