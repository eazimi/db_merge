#include "table.h"
#include <sstream>
#include <utility>
#include "dbcompare_defines.hpp"
#include "global_funcs.hpp"

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

} // namespace Kaco