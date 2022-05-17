#include "table.h"
#include <sstream>
#include <utility>
#include <stack>
#include <unordered_map>
#include <cstring>
#include "global_defines.hpp"
#include "global_funcs.hpp"
#include "table_funcs.hpp"

namespace Kaco
{
    Table::Table(const shared_ptr<IDbReader> &main_db, const shared_ptr<IDbReader> &ref_db)
    {
        m_main_db = main_db;
        m_ref_db = ref_db;
    }

    Table::Table(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db, shared_ptr<IDbReader> base_db)
    {
        m_main_db = main_db;
        m_ref_db = ref_db;
        
        m_db[DB_IDX::local] = main_db;
        m_db[DB_IDX::remote] = ref_db;
        m_db[DB_IDX::base] = base_db;
    }

    void Table::init_tbls()
    {
        m_main_tbls = m_main_db->getTables();
        m_ref_tbls = m_ref_db->getTables();

        m_table[DB_IDX::local] = m_db[DB_IDX::local]->getTables();
        m_table[DB_IDX::remote] = m_db[DB_IDX::remote]->getTables();
        m_table[DB_IDX::base] = m_db[DB_IDX::base]->getTables();
    }

    void Table::init_tbl_schema()
    {
        auto schema_main = get_tbl_schema(m_main_db, m_main_tbls);
        m_main_tbl_schema = std::move(schema_main);

        auto schema_ref = get_tbl_schema(m_ref_db, m_ref_tbls);
        m_ref_tbl_schema = std::move(schema_ref);

        m_schema[DB_IDX::local] = std::move(get_tbl_schema(m_db[DB_IDX::local], m_table[DB_IDX::local]));
        m_schema[DB_IDX::remote] = std::move(get_tbl_schema(m_db[DB_IDX::remote], m_table[DB_IDX::remote]));
        m_schema[DB_IDX::base] = std::move(get_tbl_schema(m_db[DB_IDX::base], m_table[DB_IDX::base]));
    }

    PA_VS2 Table::read_tbl_db() const
    {
        return make_pair(m_main_tbls, m_ref_tbls);
    }

    T_VS3 Table::tables_db() const
    {
        return make_tuple(m_table[DB_IDX::local], m_table[DB_IDX::remote], m_table[DB_IDX::base]);
    }

    PA_MAP_S2 Table::read_tschema_db() const
    {
        return make_pair(m_main_tbl_schema, m_ref_tbl_schema);
    }

    // TODO: store table schema in vector<string>
    map<string, string> Table::get_tbl_schema(const shared_ptr<IDbReader> &db, const vector<string> &db_tbls)
    {
        map<string, string> tbl_schema = {};
        for (auto tbl : db_tbls)
        {
            auto schema = db->getTableSchema(tbl);
            stringstream ss;
            auto schema_size = schema.size();
            for (auto i = 0; i < schema_size; i++)
            {
                if (i)
                    ss << STR_SEPERATOR;
                ss << schema[i];
            }
            tbl_schema.insert(make_pair(tbl, ss.str()));
        }
        return tbl_schema;
    }

    // first: tables in the main db but not in the ref db
    // second: tables in the ref db but not in the main db
    PA_VS2 Table::diff_tname_db() const
    {
        auto diff_tbls = getDiff(m_main_tbls, m_ref_tbls);
        return make_pair(diff_tbls.first, diff_tbls.second);
    }

    // returns schema diff for a particular table
    bool Table::diff_schema_tbl(string tbl_name, pair<string, string> &schema)
    {
        auto shcema_main = m_main_tbl_schema[tbl_name];
        auto schema_ref = m_ref_tbl_schema[tbl_name];
        schema = make_pair(shcema_main, schema_ref);
        return (shcema_main.compare(schema_ref) != 0);
    }

    // returns schema diff for a the tables in the dbs
    // 0: table name, 1: table schema in the main db, 2: table schema in the ref db
    VEC_TS3 Table::diff_schema_db()
    {
        VEC_TS3 diff_schema = {};
        auto common_tbls = getIntersect(m_main_tbls, m_ref_tbls);
        for (auto tbl : common_tbls)
        {
            PS2 schema;
            bool is_different = diff_schema_tbl(tbl, schema);
            if(is_different)
                diff_schema.push_back(make_tuple(tbl, schema.first, schema.second));
        }
        return diff_schema;
    }

    string Table::create_tbl(std::string tbl_name)
    {
        string sql = "";
        auto cc = get_cc(tbl_name, m_main_db, m_ref_db);
        string ct = generate_ct(cc.second, tbl_name);

        auto cc_diff = getDiff(cc.first, cc.second); // cc.first: main, cc.second: ref
        auto diff_cdef_main = name_definition_const(cc_diff, DB_IDX::local, NDC::definition);
        auto diff_const_main = name_definition_const(cc_diff, DB_IDX::local, NDC::constraint);
        
        // check for the columns which are in the target but not in ref
        // TODO: update this part by paying attention to the value that have been read from json config file, 
        // for now it is considered as true
        bool keep_cc = true;
        string cols = generate_str(diff_cdef_main, keep_cc);
        // check for the constraints which are in the target but not in ref
        string consts = generate_str(diff_const_main, keep_cc);

        stringstream ss_ct;
        ss_ct << ct << cols << consts << ")";
        sql = ss_ct.str();
        auto pos = sql.find_last_of(",");
        sql.replace(pos, 2, "");

        return sql;
    }

    string Table::insert_into(string tbl_name)
    {
        auto cc = get_cc(tbl_name, m_main_db, m_ref_db);
        auto cname_ref = name_definition_const(cc, DB_IDX::remote, NDC::name);
        auto col_detailed = col_name_detailed(tbl_name, cname_ref, SCHEMA_REF);
        auto cc_diff = getDiff(cc.first, cc.second); // cc.first: main, cc.second: ref
        auto diff_cname_main = name_definition_const(cc_diff, DB_IDX::local, NDC::name);
        auto diff_cname_detailed = col_name_detailed(tbl_name, diff_cname_main, SCHEMA_MAIN); 
        auto str_sel = generate_sel(col_detailed);

        // check for the columns which are in the target but not in ref
        // TODO: update this part by paying attention to the value that have been read from json config file, 
        // for now it is considered as true
        bool keep_cc = true;
        auto cols_diff = generate_str(diff_cname_detailed, keep_cc);
        string select_cmd = generate_from(tbl_name, str_sel, cols_diff); // SELECT [...] FROM [...]
        auto cc_shared = getIntersect(cc.first, cc.second);
        if(!cc_shared.empty())
        {
            auto str_join = generate_join(tbl_name, cc_shared);
            select_cmd.append(str_join);
        }
        else
        {
            auto space_pos = select_cmd.find_last_of(" ");
            select_cmd.replace(space_pos, 1, "");
        }
        stringstream ss;
        ss << "INSERT INTO " << tbl_name.append("_tmp") << " " << select_cmd << ";";
        return ss.str();
    }

    string Table::table_pk(string tbl_name, DB_IDX db_idx)
    {
        auto tbl_schema = m_schema[db_idx][tbl_name];
        auto ch_tbl_schema = const_cast<char *>(tbl_schema.c_str());
        char *token = strtok(ch_tbl_schema, STR_SEPERATOR);
        while (token != nullptr)
        {
            string str_token(token);
            auto pos = str_token.find_last_of(VAL_SEPERATOR);
            if ((pos != string::npos) && (token[pos + 1] != '0'))
            {
                int cname_begin = str_token.find_first_of(VAL_SEPERATOR);
                ++cname_begin;
                int cname_end = str_token.find_first_of(VAL_SEPERATOR, cname_begin);
                return str_token.substr(cname_begin, cname_end - cname_begin);
            }
            token = strtok(nullptr, STR_SEPERATOR);
        }
        return "";
    }

    vector<string> Table::table_cols(string tbl_name, DB_IDX db_idx)
    {
        vector<string> tbl_cols = {};
        auto tbl_schema = m_schema[db_idx][tbl_name];
        auto ch_tbl_schema = const_cast<char *>(tbl_schema.c_str());
        char *token = strtok(ch_tbl_schema, STR_SEPERATOR);
        while (token != nullptr)
        {
            string str_token(token);
            int cname_begin = str_token.find_first_of(VAL_SEPERATOR);
            ++cname_begin;
            int cname_end = str_token.find_first_of(VAL_SEPERATOR, cname_begin);
            auto col_name = str_token.substr(cname_begin, cname_end - cname_begin);
            tbl_cols.push_back(std::move(col_name));
            token = strtok(nullptr, STR_SEPERATOR);
        }
        return tbl_cols;
    }

} // namespace Kaco