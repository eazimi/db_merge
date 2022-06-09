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
        records{move(other.records)},
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
        records = other.records;
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
        records = move(other.records);
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
        oss << string(1, '"') << msg_text;
        if(schema.empty())
            oss << "[" << tbl_name << "]" << string(1, '"') << string(1, '\n');
        else
            oss << "[" << schema << "::" << tbl_name << "]" << string(1, '"') << string(1, '\n');
        int set_size = records.size();
        int data_size = records[0].size();
        bool print_cols = true;
        for (auto i = 0; i < data_size; i++)
        {
            if(print_cols)
            {
                oss << string(indent, ' ') << col_names << endl;
                print_cols = (set_size > 1);
            }                
            
            if(!captions[0].empty())
                oss << captions[0] << ": ";
            oss << records[0][i] << endl;
            if(set_size > 1)
            {
                if(!captions[1].empty())
                    oss << captions[1] << ": ";
                oss << records[1][i] << endl;
            }
            if(set_size > 1)
                oss << endl;
        }
        if(set_size == 1)
            oss << endl;
        return oss.str();
    }
} // namespace Kaco