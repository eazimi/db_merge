#include "log.h"
#include "log_builder.hpp"

namespace Kaco
{
    Log::Log(Log &&other)
        : indent{move(other.indent)},
        schema{move(other.schema)},
        tbl_name{move(other.tbl_name)},
        msg_text{move(other.msg_text)},
        col_names{move(other.col_names)},
        caption_text{move(other.caption_text)},
        caption_value{move(other.caption_value)},
        oss{move(other.oss)}
    {
    }

    Log::Log(const Log &other)
    {
        indent = other.indent;
        schema = other.schema;
        tbl_name = other.tbl_name;
        msg_text = other.msg_text;
        col_names = other.col_names;
        caption_text = other.caption_text;
        caption_value = other.caption_value;
        oss.str("");
        oss << other.oss.str();
    }    

    Log &Log::operator=(Log &&other)
    {
        if (this == &other)
            return *this;
        indent = move(other.indent);
        schema = move(other.schema);
        tbl_name = move(other.tbl_name);
        msg_text = move(other.msg_text);
        col_names = move(other.col_names);
        caption_text = move(other.caption_text);
        caption_value = move(other.caption_value);
        oss = move(other.oss);
        return *this;
    }

    LogBuilder Log::create(int indent)
    {
        return {indent};
    }

    string Log::str() const
    {
        return oss.str();
    }
} // namespace Kaco