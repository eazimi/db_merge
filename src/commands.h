#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <vector>
#include <string>
#include "table.h"
#include "IDbReader.hpp"
#include "global_defines.hpp"
#include "data_types.hpp"

using namespace std;

namespace Kaco
{
    class Commands
    {
    private:
        vector<string> new_records(const shared_ptr<IDbReader> &db, string tbl_name, string tbl_pk, DB_IDX db_idx1, DB_IDX db_idx2);

    public:
        Commands() = default;
        // get the differences by finding records that are in the db_idx1.tbl_name but not in db_idx2.tbl_name
        vector<string> diff_records(const shared_ptr<IDbReader> &db, string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2);
        PA_VS2 records_status(const shared_ptr<IDbReader> &db, string tbl_name, string primary_key);
    };

} // namespace Kaco

#endif