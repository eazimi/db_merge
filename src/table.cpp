#include "table.h"
#include <sstream>
#include <utility>
#include <stack>
#include <unordered_map>
#include "dbcompare_defines.hpp"
#include "global_funcs.hpp"
#include "table_funcs.hpp"

namespace Kaco
{
    Table::Table(const shared_ptr<IDbReader> &main_db, const shared_ptr<IDbReader> &ref_db)
    {
        m_main_db = main_db;
        m_ref_db = ref_db;
    }

    void Table::initDbTbls()
    {
        m_main_tbls = m_main_db->getTables();
        m_ref_tbls = m_ref_db->getTables();
    }

    void Table::initDbTblSchema()
    {
        auto schema_main = get_tbl_schema(m_main_db, m_main_tbls);
        m_main_tbl_schema = std::move(schema_main);

        auto schema_ref = get_tbl_schema(m_ref_db, m_ref_tbls);
        m_ref_tbl_schema = std::move(schema_ref);
    }

    PA_VS2 Table::readDbTable() const
    {
        return make_pair(m_main_tbls, m_ref_tbls);
    }

    PA_MAP_S2 Table::readDbTblSchema() const
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
    PA_VS2 Table::diffTblNameDb() const
    {
        auto diff_tbls = getDiff(m_main_tbls, m_ref_tbls);
        return make_pair(diff_tbls.first, diff_tbls.second);
    }

    // returns schema diff for a particular table
    bool Table::diffSchemaSingleTbl(string tbl_name, pair<string, string> &schema)
    {
        auto shcema_main = m_main_tbl_schema[tbl_name];
        auto schema_ref = m_ref_tbl_schema[tbl_name];
        schema = make_pair(shcema_main, schema_ref);
        return (shcema_main.compare(schema_ref) != 0);
    }

    // returns schema diff for a the tables in the dbs
    // 0: table name, 1: table schema in the main db, 2: table schema in the ref db
    VEC_TS3 Table::diffSchemaDb()
    {
        VEC_TS3 diff_schema = {};
        auto common_tbls = getIntersect(m_main_tbls, m_ref_tbls);
        for (auto tbl : common_tbls)
        {
            PS2 schema;
            bool is_different = diffSchemaSingleTbl(tbl, schema);
            if(is_different)
                diff_schema.push_back(make_tuple(tbl, schema.first, schema.second));
        }
        return diff_schema;
    }

    string Table::createTbl(std::string tbl_name)
    {
        string sql = "";
        auto cc = get_cc(tbl_name, m_main_db, m_ref_db);
        auto cc_diff = getDiff(cc.first, cc.second); // cc.first: main, cc.second: ref
        auto split_diff = split_cc_diff(cc_diff);
                
        auto diff_colname_main = get<0>(split_diff.first);
        auto diff_coldef_main = get<1>(split_diff.first);
        auto diff_const_main = get<2>(split_diff.first);

        string ct = generate_ct(cc.second, tbl_name);
        // check for the columns which are in the target but not in ref
        // TODO: update this part by paying attention to the value that have been read from json config file, 
        // for now it is considered as true
        bool keep_cc = true;
        string cols = generate_str(diff_colname_main, keep_cc);
        // check for the constraints which are in the target but not in ref
        string consts = generate_str(diff_const_main, keep_cc);

        stringstream ss_ct;
        ss_ct << ct << cols << consts << ")";
        sql = ss_ct.str();
        auto pos = sql.find_last_of(",");
        sql.replace(pos, 2, "");

        return sql;
    }

        // // split columns and constraints 
        // auto pairRefColsCons = getColsAndConstraints(refColsCons);
        // auto umapRefCols = pairRefColsCons.first;
        // auto refConstraints = pairRefColsCons.second;

        // auto refColNames = getColNamesDetails(umapRefCols).first;
        // auto detailedRefCols = updateColNames(refColNames, SCHEMA_REF, tblName);

        // print<vector<string>>("-> detailedRefCols", detailedRefCols);

        // auto detailedDiffTargetCols = updateColNames(diffTargetColNames, SCHEMA_MAIN, tblName);
        
        // print<vector<string>>("-> detailedDiffTargetCols", detailedDiffTargetCols);
        
        // stringstream ss_sel;
        // ss_sel << "SELECT ";
        // for (const auto &rec : detailedRefCols)
        //     ss_sel << rec << ", ";

        // // check for the columns which are in the target but not in ref
        // for (auto i = 0; i < diffcols_size; i++)
        // {
        //     if (keepColConst)
        //         ss_sel << detailedDiffTargetCols[i] << ", ";
        // }
        
        // auto partialInsCmd = ss_sel.str();
        // ss_sel.str("");
        // auto virgool_pos = partialInsCmd.find_last_of(",");
        // partialInsCmd.replace(virgool_pos, 1, "");

        // ss_sel << partialInsCmd << "FROM "
        //        << "\"" << SCHEMA_REF << "\"" << "." << tblName << " ";

        // if (!sharedColsCons.empty())
        // {
        //     auto sharedCols = getColsAndConstraints(sharedColsCons).first;
        //     auto sharedColNames = getColNamesDetails(sharedCols).first;
        //     stringstream ss_shared_cols;
        //     for (auto col : sharedColNames)
        //     {
        //         ss_shared_cols << "\"" << SCHEMA_REF << "\"" << "." << tblName << "." 
        //                        << col << " = " 
        //                        << "\"" << SCHEMA_MAIN << "\"" << "." << tblName << "."
        //                        << col << " AND ";
        //     }
        //     auto join = ss_shared_cols.str();
        //     ss_shared_cols.str("");
        //     if(!sharedColNames.empty())
        //     {
        //         auto and_pos = join.find_last_of("AND");
        //         join.replace(and_pos - 3, 5, ""); // remove " AND "
        //     }

        //     ss_sel << "LEFT JOIN " << "\"" << SCHEMA_MAIN << "\"" << "." << tblName
        //            << " ON " << join << ";";
        // }
        // else
        // {
        //     auto sel = ss_sel.str();
        //     ss_sel.str("");         
        //     auto space_pos = sel.find_last_of(" ");
        //     sel.replace(space_pos, 1, "");
        //     ss_sel << sel << ";";
        // }

        // stringstream ss_ins;
        // ss_ins << "INSERT INTO " << newTblName << " " << ss_sel.str();

        // print<vector<string>>("-> insert command", {ss_ins.str()});    

} // namespace Kaco