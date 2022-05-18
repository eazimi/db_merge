#include "commands.h"
#include <sstream>
#include <iostream>
#include <tuple>
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
        auto records = db->sql_exec(ss.str());
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
        auto records = db->sql_exec(ss.str());
        return records;
    }

    bool Commands::check_src(const shared_ptr<IDbReader> &db, string tbl_name, VEC_PS2 col_val, DB_IDX db_idx)
    {
        stringstream ss;
        int cv_size = col_val.size();
        for (auto i = 0; i < cv_size; i++)
        {
            string value = col_val[i].second;            
            // if(value.empty())
            //     continue;
            ss << col_val[i].first << "="
               << "\"" << value << "\""
               << " AND ";
        }
        string where_part = ss.str();
        int pos = where_part.find_last_of("AND");
        where_part.replace(pos - 3, 5, "");
        ss.str("");
        ss << "SELECT COUNT(*) FROM " << DB_ALIAS[db_idx]
           << "." << tbl_name
           << " WHERE " << where_part << ";";
        cout << ss.str() << endl;
        auto cnt = db->sql_exec(ss.str());
        return (cnt[0] == "1");
    }

    vector<string> Commands::update_records(const vector<string> &diff_recs, const vector<string> &new_recs)
    {
        vector<string> diff_fmt = {}, new_fmt = {};
        auto map_diff = format_recs(diff_recs, diff_fmt);
        auto map_new = format_recs(new_recs, new_fmt);
        auto update_fmt = getDiff(diff_fmt, new_fmt);
        auto update = retrieve_records(update_fmt.first, map_diff);
        return update;
    }    

    // returns tuple(diff between remote and local, values to be used to update records in local, 
    // diff between local and remote)
    T_VS3 Commands::records_status(const shared_ptr<IDbReader> &db, string tbl_name, vector<string> tbl_cols, string primary_key)
    {
        auto diff_remote_base = diff_records(db, tbl_name, DB_IDX::remote, DB_IDX::base);
        auto diff_local_base = diff_records(db, tbl_name, DB_IDX::local, DB_IDX::base);
        auto diff_remote_local = diff_records(db, tbl_name, DB_IDX::remote, DB_IDX::local);
        // print("-> diff_remote_local", diff_remote_local);
        auto diff_local_remote = diff_records(db, tbl_name, DB_IDX::local, DB_IDX::remote);
        
        bool remote_local_change = !diff_remote_local.empty();
        bool remote_base_change = !diff_remote_base.empty();
        bool local_base_change = !diff_local_base.empty();
        
        auto new_remote_local = new_records(db, tbl_name, primary_key, DB_IDX::remote, DB_IDX::local);
        auto new_local_remote = new_records(db, tbl_name, primary_key, DB_IDX::local, DB_IDX::remote);
        auto update_local = update_records(diff_remote_local, new_remote_local);
        record_origin(db, update_local, tbl_name, tbl_cols, {DB_IDX::remote, DB_IDX::local});
        
        return make_tuple(new_remote_local, update_local, new_local_remote);
    }

    vector<DB_IDX> Commands::record_origin(const shared_ptr<IDbReader> &db, 
                                        const vector<string> &records, string tbl_name,
                                        const vector<string> &cols, pair<DB_IDX, DB_IDX> db_idx)
    {
        for (auto rec : records)
        {
            auto col_val = match_col_val(rec, cols);
            // print(col_val, "col_val", "", "", true);
            cout << "in the local: " << check_src(db, tbl_name, col_val, DB_IDX::local) << endl;
            cout << "in the remote: " << check_src(db, tbl_name, col_val, DB_IDX::remote) << endl;
        }

        auto cols_size = cols.size();
        for (auto i = 0; i < cols_size; i++)
        {
        }
        return {};
    }

} // namespace Kaco