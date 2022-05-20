#include "commands.h"
#include <sstream>
#include <iostream>
#include "global_funcs.hpp"
#include "commands_funcs.hpp"

namespace Kaco
{
    // get the differences by finding records that are in the db_idx1.tbl_name but not in db_idx2.tbl_name
    vector<string> Commands::diff_records(const shared_ptr<IDbReader> &db, string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2)
    {
        stringstream ss;
        ss << "SELECT * FROM " << DB_ALIAS[db_idx1]
           << "." << tbl_name
           << " EXCEPT SELECT * FROM " << DB_ALIAS[db_idx2]   
           << "." << tbl_name
           << ";";
        vector<string> records;
        db->sql_exec(ss.str(), cb_sql_exec, &records);
        return records;
    }
    
    vector<string> Commands::new_records(const shared_ptr<IDbReader> &db, string tbl_name, string tbl_pk, DB_IDX db_idx1, DB_IDX db_idx2)
    {
        auto alias1 = DB_ALIAS[db_idx1];
        auto alias2 = DB_ALIAS[db_idx2];
        stringstream ss;
        ss << alias1 << "." << tbl_name << "." << tbl_pk;
        auto pk1 = ss.str();
        ss.str("");
        ss << alias2 << "." << tbl_name << "." << tbl_pk;
        auto pk2 = ss.str();
        ss.str("");
        ss << "SELECT * FROM " << alias1
           << "." << tbl_name
           << " LEFT JOIN " << alias2
           << "." << tbl_name << " ON " 
           << pk1 << "=" << pk2
           << " WHERE " << pk2 << " IS NULL;";
        vector<string> records;
        db->sql_exec(ss.str(), cb_sql_exec, &records);
        return records;
    }

    vector<string> Commands::modified_records(const vector<string> &diff_recs, const vector<string> &new_recs)
    {
        vector<string> diff_fmt = {}, new_fmt = {};
        auto map_diff = format_recs(diff_recs, diff_fmt);
        auto map_new = format_recs(new_recs, new_fmt);
        auto update_fmt = getDiff(diff_fmt, new_fmt);
        auto update = retrieve_records(update_fmt.first, map_diff);
        return update;
    }

    // arguments: (pair<db, db_idx>, pair<tbl_name, tbl_cols>, record_value)
    int Commands::delete_record(pair<const shared_ptr<IDbReader> &, DB_IDX> db_param, pair<string, vector<string>> tbl_param,
                                string rec_values)
    {
        auto col_val = match_col_val(rec_values, tbl_param.second);
        string str_cv = col_equal_val(col_val, true);
        stringstream ss_del;
        ss_del << "DELETE FROM " << DB_ALIAS[db_param.second]
               << "." << tbl_param.first << " WHERE "
               << str_cv;
        cout << "delete_record" << endl << ss_del.str() << endl;
        auto rc = db_param.first->sql_exec(ss_del.str(), nullptr, nullptr);
        return rc;
    }

    // returns tuple(diff between remote and local, values to be used to update records in local, 
    // diff between local and remote)
    PA_PA_VS2 Commands::records_status(const shared_ptr<IDbReader> &db, string tbl_name, vector<string> tbl_cols, string primary_key)
    {
        auto diff_remote_base = diff_records(db, tbl_name, DB_IDX::remote, DB_IDX::base);
        auto diff_local_base = diff_records(db, tbl_name, DB_IDX::local, DB_IDX::base);
        auto diff_remote_local = diff_records(db, tbl_name, DB_IDX::remote, DB_IDX::local);
        auto diff_local_remote = diff_records(db, tbl_name, DB_IDX::local, DB_IDX::remote);
        
        bool remote_local_change = !diff_remote_local.empty();
        bool remote_base_change = !diff_remote_base.empty();
        bool local_base_change = !diff_local_base.empty();
        
        auto new_remote_local = new_records(db, tbl_name, primary_key, DB_IDX::remote, DB_IDX::local);
        auto new_local_remote = new_records(db, tbl_name, primary_key, DB_IDX::local, DB_IDX::remote);
        auto modified_remote = modified_records(diff_remote_local, new_remote_local);
        auto modified_local = modified_records(diff_local_remote, new_local_remote);

        // insert into local
        /*
            get new records to remote, extract values from the record, create insert command the run it
        */

        // delete from local
        /*
            get the new records to the local, check if they exitst in the base table then delete them
        */

        // update in local
        /*
            find the record in the local that corresponds to a sample record picked from modified records in the remote
            according to the pk

            create the update command for the local table, use the extracted values from the record that comes from
            modified records in the remote in the set clause, use the extracted values from the record that comes
            from modified records in the local in the where clause

            run update command
        */

        map<string, string> map_modified_local = map_col_record(modified_local);
        stringstream ss_del, ss_ins;

        for (auto record : modified_remote)
        {
            auto col_val_remote = match_col_val(record, tbl_cols);
            auto pk_value = col_val_remote[0].second;
            string rec_local = (map_modified_local.find(pk_value))->second;

            auto str_cv_remote = col_val_par(col_val_remote, true);

            delete_record({db, DB_IDX::local}, {tbl_name, tbl_cols}, rec_local);

            ss_ins << "INSERT INTO " << DB_ALIAS[DB_IDX::local]
                   << "." << tbl_name << " "
                   << str_cv_remote.first << " VALUES "
                   << str_cv_remote.second;
        }

        db->sql_exec(ss_ins.str(), nullptr, nullptr);

        return make_pair(make_pair(new_remote_local, new_local_remote), make_pair(modified_remote, modified_local));
    }

} // namespace Kaco