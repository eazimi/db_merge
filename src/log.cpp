#include "log.h"
#include "log_builder.hpp"

namespace Kaco
{
    static string full_tbl(const string &schema, const string &tbl_name)
    {
        ostringstream ss;
        if(schema.empty())
            ss << "[" << tbl_name << "]";
        else
            ss << "[" << schema << "::" << tbl_name << "]";
        return ss.str();
    }

    static string format_caption(string caption)
    {
        ostringstream ss;
        ss.str("");
        if (!caption.empty())
            ss << caption << ": ";
        return ss.str();
    }

    Log::Log(Log &&other)
        : indent{move(other.indent)},
        schema{move(other.schema)},
        tbl_name{move(other.tbl_name)},
        msg_text{move(other.msg_text)},
        col_names{move(other.col_names)},
        data{move(other.data)},
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
        data = other.data;
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
        data = move(other.data);
        captions = move(other.captions);
        oss = move(other.oss);
        return *this;
    }

    unique_ptr<LogBuilder> Log::create(int indent)
    {
        return make_unique<LogBuilder>(indent);
    }

    /* 
        msg_text [schema::tbl_name]
        [indent] col_names
        caption_text: caption_value
        ...
        caption_text: caption_value 
    */
    string Log::str_records()
    {
        oss << string(1, '"') << msg_text << full_tbl(schema, tbl_name) << string(1, '"') << endl;
        int set_size = data.size();
        int data_size = data[0].size();
        for (auto i = 0; i < data_size; i++)
        {
            if (i == 0 || set_size > 1)
                oss << string(indent, ' ') << col_names << endl;
            oss << format_caption(captions[0]) << data[0][i] << endl;
            if(set_size > 1)
                oss << format_caption(captions[1])
                    << data[1][i] << string(2, '\n');
        }
        if (!(data_size > 0 && set_size > 1))
            oss << endl;
        return oss.str();
    }

    string Log::str_no_pk()
    {
        ostringstream ss;
        auto ftbl = full_tbl(schema, tbl_name);
        ss << "-> no PK found for " << ftbl << ", "
            << "it is not possible to seperate modified records from new records, "
            << "here is all the new/modified records in " << ftbl;
        oss << string(1, '"') << ss.str() << string(1, '"') << endl;
        int data_size = data[0].size();
        oss << string(indent, ' ') << col_names << endl;
        for (auto i = 0; i < data_size; i++)
        {
            oss << format_caption(captions[0])
                << data[0][i] << endl;
        }
        oss << endl;
        return oss.str();
    }

    string Log::str_diff_tbls()
    {
        if(msg_multi.size() > 1)
            oss << string(1, '"') << msg_multi[0] << schema << msg_multi[1] << string(1, '"') << endl;
        int data_size = data[0].size();
        for(auto i=0; i<data_size; i++)
            oss << data[0][i] << endl;
        oss << endl;
        return oss.str();
    }
} // namespace Kaco