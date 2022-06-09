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
        captions{move(other.captions)},
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
        captions = other.captions;
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
        captions = move(other.captions);
        oss = move(other.oss);
        return *this;
    }

    LogBuilder Log::create(int indent)
    {
        return {indent};
    }

    /* 
        msg_text [schema::tbl_name]
        [indent] col_names
        caption_text: caption_value
        ...
        caption_text: caption_value 
    */
    string Log::str()
    {
        oss.str("");
        oss << msg_text
            << "[" << schema << "::" << tbl_name << "]" << endl
            << string(' ', indent) << col_names << endl;
        for(auto cap:captions)
            oss << cap.first << ": " << cap.second << endl;
        return oss.str();
    }
} // namespace Kaco