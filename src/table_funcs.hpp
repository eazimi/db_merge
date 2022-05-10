#ifndef TABLE_FUNCS
#define TABLE_FUNCS

#include "data_types.hpp"

namespace Kaco
{
    // it assumes that there is no disparity between the number of '(' and ')'
    static bool process_word(string word, stack<char> &stack)
    {
        bool stackIsEmpty = false;
        bool virgool = false;
        for (auto ch : word)
        {
            if (ch == '(')
                stack.push('(');
            else if (ch == ')')
                stack.pop();
            else if (ch == ',')
            {
                virgool = true;
                stackIsEmpty = stack.empty();
            }
        }
        return (stackIsEmpty && virgool);
    };

    static vector<string> process_ct(string data)
    {
        vector<string> cols = {};
        int space_pos = 0;
        stack<char> stack;
        stringstream ss;
        space_pos = data.find_first_of(" ");
        while (space_pos != string::npos)
        {
            auto word = data.substr(0, space_pos);
            bool flush = process_word(word, stack);
            if (flush)
            {
                ss << word.substr(0, word.length() - 1);
                cols.push_back(std::move(ss.str()));
                ss.str("");
            }
            else
                ss << word << " ";
            data = data.substr(space_pos + 1);
            space_pos = data.find_first_of(" ");
        }
        ss << data;
        cols.push_back(ss.str());
        return cols;
    };

    static vector<string> split_ct(string cmd)
    {
        int first_pos = cmd.find_first_of("(");
        int last_pos = cmd.find_last_of(")");
        auto data = cmd.substr(first_pos + 1, last_pos - first_pos - 1);
        auto cols = process_ct(data);
        return cols;
    }

    static PA_VS2 get_cc(string tbl_name,
                         const shared_ptr<IDbReader> &db_main,
                         const shared_ptr<IDbReader> &db_ref)
    {
        // db1: target, db2: reference
        auto main_ct = db_main->getCreateTblCmd(tbl_name);
        auto ref_ct = db_ref->getCreateTblCmd(tbl_name);
        // columns and constraints
        auto main_cc = split_ct(main_ct);
        auto ref_cc = split_ct(ref_ct);
        return make_pair(main_cc, ref_cc);
    }

    static T_VS3 split_cc(vector<string> col_con)
    {
        vector<string> col_name = {};
        vector<string> col_detail = {};
        vector<string> consts = {};
        for (auto cc : col_con)
        {
            int pos = cc.find_first_of(" ");
            auto name = cc.substr(0, pos);
            bool is_const = CHECK_IS_TBL_CONSTRAINT(name);
            if (!is_const)
            {
                col_name.push_back(std::move(name));
                col_detail.push_back(std::move(cc));
            }                
            else
                consts.push_back(std::move(cc));
        }
        return make_tuple(col_name, col_detail, consts);
    }

    static vector<string> col_name_detailed(string tbl_name, vector<string> cols, string schema)
    {
        vector<string> detailed_cols = {};
        stringstream ss;
        for (auto col : cols)
        {
            ss << "\"" << schema << "\""
               << "." << tbl_name << "." << col;
            detailed_cols.push_back(std::move(ss.str()));
            ss.str("");
        }
        return detailed_cols;
    }

    // ndc: 0 -> col name, 1 -> col detail, 2 -> constraints
    static vector<string> name_definition_const(PA_VS2 cc, DB main_ref, NDC ndc)
    {
        bool condition = (cc.first.empty() && (main_ref == DB::main)) ||
                         (cc.second.empty() && (main_ref == DB::ref)) ||
                         ((main_ref != DB::main) && (main_ref != DB::ref)) ||
                         ((ndc != NDC::name) && (ndc != NDC::definition) && (ndc != NDC::constraint));
        if (condition)
            return {};
        auto cc_picked = (main_ref == DB::main) ? cc.first : cc.second;
        auto split = split_cc(cc_picked);
        vector<string> data = (ndc == NDC::name) ? get<0>(split) : ((ndc == NDC::definition) ? get<1>(split) : get<2>(split));
        return data;
    }

    // ndc: 0 -> col name, 1 -> col detail, 2 -> constraints 
    static vector<string> name_definition_const(vector<string> cc, short ndc)
    {
        if (cc.empty() || ((ndc != NDC::name) && (ndc != NDC::definition) && (ndc != NDC::constraint)))
            return {};
        auto split = split_cc(cc);
        vector<string> data = (ndc == NDC::name) ? get<0>(split) : 
                              ((ndc == NDC::definition) ? get<1>(split) : get<2>(split));
        return data;
    }

    static string generate_ct(vector<string> col_con, std::string tbl_name)
    {
        stringstream ss;
        ss << "CREATE TABLE " << tbl_name.append("_tmp") << " (";
        for (auto const &cc : col_con)
            ss << cc << ", ";
        return ss.str();
    }

    static string generate_sel(vector<string> cols)
    {
        stringstream ss_sel;
        ss_sel << "SELECT ";
        for (auto col : cols)
            ss_sel << col << ", ";
        return ss_sel.str();
    }

    static string generate_from(string tbl_name, string sel, string cols)
    {
        stringstream ss;
        ss << sel << cols;
        string ss_str = ss.str();
        ss.str("");
        auto virgool_pos = ss_str.find_last_of(",");
        ss_str.replace(virgool_pos, 1, "");
        ss << ss_str << "FROM "
           << "\"" << SCHEMA_REF << "\"" 
           << "." << tbl_name << " ";
        return ss.str();
    }

    static string generate_str(vector<string> cc, bool keep_cc)
    {
        int cc_size = cc.size();
        stringstream ss;
        for (auto i = 0; i < cc_size; i++)
        {
            if (keep_cc)
                ss << cc[i] << ", ";
        }
        return ss.str();
    }

    static string generate_join(string tbl_name, vector<string> cc)
    {
        vector<string> col_names = name_definition_const(cc, NDC::name);
        stringstream ss;
        for (auto cname : col_names)
        {
            ss << "\"" << SCHEMA_REF << "\""
               << "." << tbl_name << "."
               << cname << " = "
               << "\"" << SCHEMA_MAIN << "\""
               << "." << tbl_name << "."
               << cname << " AND ";
        }
        auto str_join = ss.str();        
        if (str_join != "")
        {
            auto and_pos = str_join.find_last_of("AND");
            str_join.replace(and_pos - 3, 5, ""); // remove " AND "
        }
        ss.str("");
        ss << "LEFT JOIN "
               << "\"" << SCHEMA_MAIN << "\""
               << "." << tbl_name
               << " ON " << str_join;
        return ss.str();         
    }

} // namespace Kaco

#endif