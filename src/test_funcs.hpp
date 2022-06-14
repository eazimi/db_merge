#ifndef TEST_FUNCS_HPP
#define TEST_FUNCS_HPP

#include <string>
#include <tuple>
#include "dbcompare.h"
#include "global_funcs.hpp"
#include "global_defines.hpp"
#include "log.h"
#include "log_builder.hpp"

using namespace std;
using namespace Kaco;

namespace Test
{
    static void test_records_status(DbCompare *db)
    {        
        auto common_tbls = db->common_tbls_db(DB_IDX::local, DB_IDX::remote);
        for (auto p : common_tbls)
        {
            auto tbl_name = p.first;
            auto tbl_cols = db->table_cols(tbl_name, DB_IDX::remote);
            auto tbl_pk_local = db->table_pk(tbl_name, DB_IDX::local);
            auto tbl_pk_remote = db->table_pk(tbl_name, DB_IDX::remote);
            auto records = db->records_status(tbl_name);
            if (tbl_pk_local.empty() || tbl_pk_remote.empty())
            {
                cout << Log::create()->add_table(DB_ALIAS[DB_IDX::remote], tbl_name)
                            .add_col_names(tbl_cols)
                            .add_records("", (get<0>(records)).first)
                            .str_no_pk();
                cout << Log::create()->add_table(DB_ALIAS[DB_IDX::local], tbl_name)
                            .add_col_names(tbl_cols)
                            .add_records("", (get<0>(records)).second)
                            .str_no_pk();
                continue;
            }
            cout << Log::create()->add_msg_text("-> new records in ")
                    .add_table(DB_ALIAS[DB_IDX::remote], tbl_name)
                    .add_col_names(tbl_cols)
                    .add_records("", (get<0>(records)).first)
                    .str_records();
            cout << Log::create()->add_msg_text("-> new records in ")
                    .add_table(DB_ALIAS[DB_IDX::local], tbl_name)
                    .add_col_names(tbl_cols)
                    .add_records("", (get<0>(records)).second)
                    .str_records();
            cout << Log::create()->add_msg_text("-> modified records in ")
                    .add_table("", tbl_name)
                    .add_col_names(tbl_cols)
                    .add_records("-> old values", (get<1>(records)).second)
                    .add_records("-> new values", (get<1>(records)).first)
                    .str_records();
        }
    }
} // namespace Test

#endif