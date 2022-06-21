#include "log.h"
#include "log_builder.hpp"
#include "global_defines.hpp"
#include <cstring>

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

    static string format_schema_string(string schema)
    {
        ostringstream oss{""};
        auto token = strtok(const_cast<char *>(schema.c_str()), STR_SEPERATOR);
        while(token != nullptr)
        {
            oss << string(1, '"') << token << string(1, '"') << ", ";
            token = strtok(nullptr, STR_SEPERATOR);
        }
        auto oss_str = oss.str();
        int virgool = oss_str.find_last_of(", ");
        oss_str.replace(virgool - 1, 2, "");
        return oss_str;
    }

    Log::Log(Log &&other)
        : indent{move(other.indent)},
          schema{move(other.schema)},
          schema_aux{move(other.schema_aux)},
          tbl_name{move(other.tbl_name)},
          msg_text{move(other.msg_text)},
          col_names{move(other.col_names)},
          data{move(other.data)},
          vec_tu_data{move(other.vec_tu_data)},
          vec_pa_data{move(other.vec_pa_data)},
          captions{move(other.captions)},
          oss{move(other.oss)}
    {
    }

    Log::Log(const Log &other)
    {
        indent = other.indent;
        schema = other.schema;
        schema_aux = other.schema_aux;
        tbl_name = other.tbl_name;
        msg_text = other.msg_text;
        col_names = other.col_names;
        data = other.data;
        vec_tu_data = other.vec_tu_data;
        vec_pa_data = other.vec_pa_data;
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
        schema_aux = move(other.schema_aux);
        tbl_name = move(other.tbl_name);
        msg_text = move(other.msg_text);
        col_names = move(other.col_names);
        data = move(other.data);
        vec_tu_data = move(other.vec_tu_data);
        vec_pa_data = move(other.vec_pa_data);
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

    string Log::str_schema()
    {
        oss << string(1, '"') << msg_multi[0] << " " << schema << msg_multi[1] << " " 
            << msg_multi[2] << " " << schema_aux << msg_multi[1] << string(1, '"') << endl;

        for (auto tu : vec_tu_data)
        {
            oss << "[" << get<0>(tu) << "]" << endl
                 << col_names << endl
                 << captions[0] << " " << schema << " " << captions[1] << format_schema_string(get<1>(tu)) << endl
                 << captions[0] << " " << schema_aux << " " << captions[1] << format_schema_string(get<2>(tu)) << string(2, '\n');
        }

        oss << string(1, '"') << msg_multi[3] << " " << schema << " " << msg_multi[4] << string(1, '"') << endl;
        for (auto p : vec_pa_data[0])
        {
            oss << "[" << p.first << "]" << endl
                 << col_names << endl
                 << format_schema_string(p.second) << string(2, '\n');
        }
        oss << string(1, '"') << msg_multi[3] << " " << schema_aux << " " << msg_multi[4] << string(1, '"') << endl;
        for (auto p : vec_pa_data[1])
        {
            oss << "[" << p.first << "]" << endl
                 << col_names << endl
                 << format_schema_string(p.second) << string(2, '\n');
        }
        
        return oss.str();
    }
} // namespace Kaco