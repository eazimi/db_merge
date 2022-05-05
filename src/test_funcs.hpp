#ifndef TEST_FUNCS_HPP
#define TEST_FUNCS_HPP

#include <string>
#include <memory>
#include "dbcompare.h"
#include "IDbReader.hpp"

using namespace std;
using namespace Kaco;

namespace Test
{
    static void test_readDbTriggers(const shared_ptr<DbCompare> &db)
    {
        db->readDbTriggers();
    }

    static void test_readSingleTblTriggers(const shared_ptr<DbCompare> &db, string table_name)
    {
        db->readSingleTblTriggers(table_name);
    }

    static void test_diffTriggerDb(const shared_ptr<DbCompare> &db)
    {
        db->diffTriggerDb();
    }

    static void test_diffTriggerSingleTbl(const shared_ptr<DbCompare> &db, string table_name)
    {
        db->diffTriggerSingleTbl(table_name);
    }

} // namespace Test

#endif