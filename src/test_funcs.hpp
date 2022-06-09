#ifndef TEST_FUNCS_HPP
#define TEST_FUNCS_HPP

#include <string>
#include "dbcompare.h"
#include "global_funcs.hpp"
#include "log.h"
#include "log_builder.hpp"

using namespace std;
using namespace Kaco;

namespace Test
{
    static void test_records_status(DbCompare *db)
    {
        auto tbl_name = "ledCfg";
        auto records = db->records_status(tbl_name);
        print<vector<string>>("-> new records in the remote", (get<0>(records)).first);
        print<vector<string>>("-> new records in the local", (get<0>(records)).second); 
        print<vector<string>>("-> modified records in the remote", (get<1>(records)).first); 
        print<vector<string>>("-> modified records in the local", (get<1>(records)).second); 
    }
} // namespace Test

#endif