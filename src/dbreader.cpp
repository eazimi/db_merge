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
        auto rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READONLY, 0);
        if (rc)
        {
            cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
            return false;
        }
        else
            cout << "Opened database successfully: " << dbPath << endl;
        return true;
    }

} // namespace Kaco