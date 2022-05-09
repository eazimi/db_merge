#ifndef TABLE_H
#define TABLE_H

#include <memory>
#include <map>
#include <string>
#include <vector>
#include "IDbReader.hpp"
#include "data_types.hpp"

using namespace std;

namespace Kaco
{
    class Table
    {
        private:
            shared_ptr<IDbReader> m_main_db, m_ref_db;
            vector<string> m_main_tbls, m_ref_tbls;
            map<string, string> m_main_tbl_schema, m_ref_tbl_schema;
            map<string, string> get_tbl_schema(const shared_ptr<IDbReader> &db, const vector<string> &db_tbls);

        public:
            Table(const shared_ptr<IDbReader> &main_db, const shared_ptr<IDbReader> &ref_db);
            void initDbTbls();
            void initDbTblSchema();
            PA_VS2 readDbTable() const;
            PA_MAP_S2 readDbTblSchema() const;
            PA_VS2 diffTblNameDb() const;
            bool diffSchemaSingleTbl(string tbl_name, pair<string, string> &schema); // returns schema diff for a particular table
            VEC_TS3 diffSchemaDb(); // returns schema diff for a the tables in the dbs
            string createTbl(string tbl_name);
            string insertInto(string tbl_name);
    };

} // namespace Kaco

#endif