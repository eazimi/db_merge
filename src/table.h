#ifndef TABLE_H
#define TABLE_H

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <array>
#include "IDbReader.hpp"
#include "data_types.hpp"
#include "global_defines.hpp"

using namespace std;

namespace Kaco
{
    class Table
    {
        private:
            IDbReader *m_main_db, *m_ref_db;
            array<IDbReader *, DB_CNT> m_db;
            vector<string> m_main_tbls, m_ref_tbls;
            array<vector<string>, DB_CNT> m_table;
            map<string, string> m_main_tbl_schema, m_ref_tbl_schema;
            array<map<string, string>, DB_CNT> m_schema;
            map<string, string> get_tbl_schema(IDbReader *db, const vector<string> &db_tbls);

        public:
            Table(IDbReader *main_db, IDbReader *ref_db);
            Table(IDbReader *main_db, IDbReader *ref_db, IDbReader *base_db);
            void init_tbls();
            void init_tbl_schema();
            PA_VS2 read_tbl_db() const;
            T_VS3 tables_db() const;
            VEC_PS2 common_tbls_db(DB_IDX db_idx1, DB_IDX db_idx2);
            vector<string> common_tnames_db(DB_IDX db_idx1, DB_IDX db_idx2);
            PA_MAP_S2 read_tschema_db() const;
            bool diff_schema_tbl(string tbl_name, pair<string, string> &schema); // returns schema diff for a particular table
            string create_tbl(string tbl_name);
            string insert_into(string tbl_name);
            vector<string> table_pk(string tbl_name, DB_IDX db_idx);
            vector<string> table_cols(string tbl_name, DB_IDX db_idx);
    };

} // namespace Kaco

#endif