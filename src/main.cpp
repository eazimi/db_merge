#include <iostream>
#include "dbreader.h"
#include "test_funcs.hpp"

using namespace std;
using namespace Kaco;
using namespace Test;

#define DEFAULT_DB
#ifdef DEFAULT_DB
constexpr char dbpath_local[] = "../files/config-app.db3";
constexpr char dbpath_remote[] = "../files/config-psd2.db3";
constexpr char dbpath_base[] = "../files/config-base.db3";
#endif

unique_ptr<DbCompare> open_db(string path_local, string path_remote, string path_base)
{
    shared_ptr<DbReader> local_db = make_shared<DbReader>();
    shared_ptr<DbReader> remote_db = make_shared<DbReader>();
    shared_ptr<DbReader> base_db = make_shared<DbReader>();
    bool db_connected = local_db->connect(path_local);
    if (!db_connected)
    {
        cout << "can't open " << path_local << " as local db" << endl;
        exit(1);
    }
    db_connected = remote_db->connect(path_remote);
    if (!db_connected)
    {
        cout << "can't open " << path_remote << " as remote db" << endl;
        exit(1);
    }
    db_connected = base_db->connect(path_base);
    if (!db_connected)
    {
        cout << "can't open " << path_base << " as base db" << endl;
        exit(1);
    }
    return make_unique<DbCompare>(local_db, remote_db, base_db);
}

bool check_args(int argc, char **argv)
{
    if (argc != 3)
    {
        // cout << "-> try it like this:"
        //      << endl
        //      << "./db_tool [PATH_TO_DB_LOCAL] [PATH_TO_DB_REMOTE] [PATH_TO_DB_BASE]"
        //      << endl;
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    string path_local = dbpath_local, path_remote = dbpath_remote, path_base = dbpath_base;
    bool chk_args = check_args(argc, argv);
    if (chk_args)
    {
        path_local = argv[1];
        path_remote = argv[2];
        path_base = argv[3];
    }
    unique_ptr<DbCompare> dbCompare = move(open_db(path_local, path_remote, path_base));
    dbCompare->initialize();
    auto rc_attach = dbCompare->attach_db(dbpath_remote, dbpath_base);
    if(rc_attach != 0)
    {
        cout << "can't attach to " << dbpath_remote << " or " 
             << dbpath_base << endl;
        exit(1);
    }

    test_records_status(dbCompare.get());

    return 0;
}