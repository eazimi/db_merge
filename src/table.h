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
            shared_ptr<IDbReader> m_main_db, m_ref_db;
            array<shared_ptr<IDbReader>, DB_CNT> m_db;
            vector<string> m_main_tbls, m_ref_tbls;
            array<vector<string>, DB_CNT> m_table;
            map<string, string> m_main_tbl_schema, m_ref_tbl_schema;
            array<map<string, string>, DB_CNT> m_schema;
            map<string, string> get_tbl_schema(const shared_ptr<IDbReader> &db, const vector<string> &db_tbls);

        public:
            Table(const shared_ptr<IDbReader> &main_db, const shared_ptr<IDbReader> &ref_db);
            Table(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db, shared_ptr<IDbReader> base_db);
            void init_tbls();
            void init_tbl_schema();
            PA_VS2 read_tbl_db() const;
            T_VS3 tables_db() const;
            PA_MAP_S2 read_tschema_db() const;
            PA_VS2 diff_tname_db() const;
            bool diff_schema_tbl(string tbl_name, pair<string, string> &schema); // returns schema diff for a particular table
            VEC_TS3 diff_schema_db(); // returns schema diff for a the tables in the dbs
            string create_tbl(string tbl_name);
            string insert_into(string tbl_name);
            string table_pk(string tbl_name, DB_IDX db_idx);
            vector<string> table_cols(string tbl_name, DB_IDX db_idx);
    };

} // namespace Kaco

#endif