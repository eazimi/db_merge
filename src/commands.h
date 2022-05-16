#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <vector>
#include <string>
#include "IDbReader.hpp"
#include "global_defines.hpp"

using namespace std;

namespace Kaco
{
    class Commands
    {
    private:

    public:
        Commands() = default;
        // get the differences by finding records that are in the db_idx1.tbl_name but not in db_idx2.tbl_name 
        vector<string> diff_records(const shared_ptr<IDbReader> &db, string tbl_name, DB_IDX db_idx1, DB_IDX db_idx2);
    };

} // namespace Kaco

#endif