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

    /* 
        msg_text [schema::tbl_name]
        [indent] col_names
        caption_text: caption_value
        ...
        caption_text: caption_value 
    */
    static void test_records_status(DbCompare *db)
    {
        auto tbl_name = "ledCfg";
        auto records = db->records_status(tbl_name);
        auto tbl_cols = db->table_cols(tbl_name, DB_IDX::remote);
        cout << Log::create()
                .add_msg_text("-> new records in ").add_table(DB_ALIAS[DB_IDX::remote], tbl_name)
                .add_col_names(tbl_cols)
                .add_records("",  (get<0>(records)).first)
                .str();

        cout << Log::create()
                .add_msg_text("-> new records in ").add_table(DB_ALIAS[DB_IDX::local], tbl_name)
                .add_col_names(tbl_cols)
                .add_records("",  (get<0>(records)).second)
                .str();

        cout << Log::create()
                .add_msg_text("-> modified records in ").add_table("", tbl_name)
                .add_col_names(tbl_cols)
                .add_records("-> old values",  (get<1>(records)).second)
                .add_records("-> new values",  (get<1>(records)).first)
                .str();
    }
} // namespace Test

#endif