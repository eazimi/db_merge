#include "dbcompare.h"
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "global_funcs.hpp"
#include "data_types.hpp"
#include "global_defines.hpp"

using namespace std;

// #define DEBUG
namespace Kaco
{
    static map<string, string> initTableIndices(const shared_ptr<IDbReader> &db, const vector<string> &tables)
    {
        map<string, string> result{};
        for (auto table : tables)
        {
            auto tableIndex = db->getIndices(table);
            stringstream ss;
            for (auto i = 0; i < tableIndex.size(); i++)
            {
                if (i)
                    ss << STR_SEPERATOR;
                ss << tableIndex[i];
            }
            result.insert({table, ss.str()});
        }
        return result;
    }

    DbCompare::DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2) : m_db1(db1), m_db2(db2), m_initialized(false)
    {
        m_trigger = make_shared<Trigger>(db1, db2);
        m_table = make_shared<Table>(db1, db2);
        INIT_MAPS;
    }

    DbCompare::~DbCompare()
    {
        CLEAR_MAPS;
    }

    bool DbCompare::initialize()
    {
        CHECK_INITIALIZED(m_initialized, MSG_ALREADY_INIT, FALSE);
        m_table->initDbTbls();
        auto db_tbls = m_table->readDbTable();
        m_trigger->initDbTriggers(db_tbls.first, db_tbls.second);
        m_table->initDbTblSchema();
        initDbTableIndices();
        m_initialized = true;
        return m_initialized;
    }

    string DbCompare::compareAndMerge()
    {
        CHECK_INITIALIZED(!m_initialized, MSG_NOT_INIT, STR_NULL);

        string result = {};
        auto main_schema = m_table->readDbTblSchema().first;
        auto ref_schema = m_table->readDbTblSchema().second;
        // db1: source -> db2: target
        for (auto tblName_schema : main_schema)
        {
            auto srcTblName = tblName_schema.first;
            // bool targetTblFound = (m_refTblSchema.find(srcTblName) != m_refTblSchema.end());
            bool targetTblFound = false;
            auto targetTblSchema = ref_schema[srcTblName];
            if (targetTblSchema != "")
                targetTblFound = true;
            if (targetTblFound)
            {
                // check for schemas
                auto srcTblSchema = tblName_schema.second;
                auto targetTblSchema = ref_schema[srcTblName];
                if (srcTblSchema == targetTblSchema)
                {
                    // check for the indices
                    auto srcTblIndices = m_db1TblIndices[srcTblName];
                    auto targetTblIndices = m_db2TblIndices[srcTblName];
                    if (srcTblIndices == targetTblIndices)
                    {
                        // check for triggers
                        // auto srcTblTriggers = m_mainTblTriggers[srcTblName];
                        // auto targetTblTriggers = m_refTblTriggers[srcTblName];
                        // if (srcTblTriggers == targetTblTriggers)
                        // {
                        //     // TODO: complete it
                        //     // check for data
                        // }
                        // else
                        // {
                        //     // TODO: complete it
                        //     // different triggers
                        //     cout << srcTblName << " is different in triggers" << endl;
                        // }
                    }
                    else
                    {
                        // TODO: complete it
                        // different indices
                        cout << srcTblName << " is different in indices" << endl;
                    }
                }
                else
                {
                    // TODO: complete it
                    // different schemas
                    cout << srcTblName << " is different in schema" << endl;
                }
            }
            else
            {
                // TODO: complete it
                // new table
                cout << srcTblName << " is new to db2" << endl;
            }
        }

        return result;
    }

    void DbCompare::testDbDump()
    {
        cout << endl
             << "-> dumping db1 in ./dump_db1.sql" << endl;
        m_db1->dbDump((char *)"./dump_db1.sql");
        cout << "-> dumping db2 in ./dump_db2.sql" << endl;
        m_db2->dbDump((char *)"./dump_db2.sql");
    }

    void DbCompare::testTableIndices()
    {
        print(m_db1TblIndices, "-> db1 all the table indices");
        print(m_db2TblIndices, "-> db2 all the table indices");
    }

    PA_VS2 DbCompare::readDbTables() const
    {
        return m_table->readDbTable();
    }

    PA_MAP_S2 DbCompare::readDbTblSchema() const
    {
        return m_table->readDbTblSchema();
    }

    PA_VEC_PS2 DbCompare::readSingleTblTriggers(string table_name) const
    {
        return m_trigger->readSingleTblTriggers(table_name);
    }

    PA_MAP_SVPS2 DbCompare::diffTriggerDb() const
    {
        auto db_tbls = m_table->readDbTable();
        return m_trigger->diffTriggerDb(db_tbls.first, db_tbls.second);
    }

    PA_VEC_PS2 DbCompare::diffTriggerSingleTbl(string table_name) const
    {
        return m_trigger->diffTriggerSingleTbl(table_name);
    }

    VEC_PS2 DbCompare::updateTriggerSingleTbl(string table_name) const
    {
        return m_trigger->updateTriggerSingleTbl(table_name);
    }

    void DbCompare::initDbTableIndices()
    {
        auto db_tbls = m_table->readDbTable();
        auto tableIndices1 = initTableIndices(m_db1, db_tbls.first);
        m_db1TblIndices = std::move(tableIndices1);

        auto tableIndices2 = initTableIndices(m_db2, db_tbls.second);
        m_db2TblIndices = std::move(tableIndices2);
    }

} // namespace Kaco
