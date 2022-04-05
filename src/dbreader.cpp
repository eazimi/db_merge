#include "dbreader.h"
#include <iostream>

namespace Kaco
{
    DBReader::~DBReader()
    {
        sqlite3_close(db);
    }

    bool DBReader::connect(string dbPath)
    {
        this->dbPath = "";
        auto rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READONLY, 0);
        if (rc)
        {
            cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
            return false;
        }
        else
            cout << "Opened database successfully: " << dbPath << endl;
        this->dbPath = dbPath;
        return true;
    }

    int DBReader::sqlExec(string sql, ExecCallback cb)
    {
        char *zErrMsg = 0;
        auto rc = sqlite3_exec(db, sql.c_str(), cb, this, &zErrMsg);

        if (SQLITE_OK != rc)
            cout << __FUNCTION__ << " returned error code " << rc << ": " << zErrMsg << " for " << dbPath;

        if (zErrMsg)
            sqlite3_free(zErrMsg);

        return rc;
    }

} // namespace Kaco