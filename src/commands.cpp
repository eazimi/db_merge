#include "commands.h"
#include <sstream>
#include <iostream>
#include <utility>

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
        cout << ss.str() << endl;
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

    PA_VS2 Commands::records_status(const shared_ptr<IDbReader> &db, string tbl_name, string primary_key)
    {
        auto new_rec_remote = new_records(db, tbl_name, primary_key, DB_IDX::remote, DB_IDX::local);
        auto new_rec_local = new_records(db, tbl_name, primary_key, DB_IDX::local, DB_IDX::remote);
        return make_pair(new_rec_remote, new_rec_local);
    }

} // namespace Kaco