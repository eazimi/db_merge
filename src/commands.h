#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <vector>
#include <string>
#include <utility>
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
        vector<string> new_records(IDbReader *db, string tbl_name,
                                   vector<string> tbl_pk, DB_IDX db_idx1, DB_IDX db_idx2);
        vector<string> modified_records(const vector<string> &diff_recs, const vector<string> &new_recs);
        vector<DB_IDX> record_origin(IDbReader *db, const vector<string> &records, string tbl_name,
                                     const vector<string> &cols, pair<DB_IDX, DB_IDX> db_idx);
        // arguments: (pair<db, db_idx>, pair<tbl_name, tbl_cols>, record_value)
        int delete_record(pair<IDbReader *, DB_IDX> db_param, pair<string, vector<string>> tbl_param,
                          string rec_values);
        // arguments: (pair<db, db_idx>, pair<tbl_name, tbl_cols>, record_value)
        int insert_record(pair<IDbReader *, DB_IDX> db_param, pair<string, vector<string>> tbl_param,
                          string rec_values);

    public:
        Commands() = default;
        // get the differences by finding records that are in the db_idx1.tbl_name but not in db_idx2.tbl_name
        vector<string> diff_records(IDbReader *db, string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2);
        // returns pair<pair<new records in remote, new records in local>, 
        //    pair<modified records in remote, modified records in local>>
        PA_PA_VS2 records_status(IDbReader *db, string tbl_name, vector<string> tbl_cols, vector<string> primary_key);
    };

} // namespace Kaco

#endif