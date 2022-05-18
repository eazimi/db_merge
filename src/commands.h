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
        vector<string> new_records(const shared_ptr<IDbReader> &db, string tbl_name,
                                   string tbl_pk, DB_IDX db_idx1, DB_IDX db_idx2);
        vector<string> modified_records(const vector<string> &diff_recs, const vector<string> &new_recs);
        vector<DB_IDX> record_origin(const shared_ptr<IDbReader> &db, const vector<string> &records, string tbl_name,
                                     const vector<string> &cols, pair<DB_IDX, DB_IDX> db_idx);
    public:
        Commands() = default;
        // get the differences by finding records that are in the db_idx1.tbl_name but not in db_idx2.tbl_name
        vector<string> diff_records(const shared_ptr<IDbReader> &db, string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2);
        // returns tuple(diff between remote and local, values to be used to update records in local, 
        // diff between local and remote)
        PA_PA_VS2 records_status(const shared_ptr<IDbReader> &db, string tbl_name, vector<string> tbl_cols, string primary_key);
    };

} // namespace Kaco

#endif