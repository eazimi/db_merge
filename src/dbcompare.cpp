#include "dbcompare.h"
#include <iostream>
#include <sstream>

using namespace std;

#define STR_SEPERATOR "##"
#define VAL_SEPERATOR "|"
#define STR_NULL std::string("")
#define FALSE false
#define MSG_ALREADY_INIT "DbCompare is already initialized."
#define MSG_NOT_INIT "DbCompare is not initialized. Call initialize() first!"

#define INIT_VECTORS  \
    m_db1Tables = {}; \
    m_db2Tables = {};

#define INIT_MAPS                               \
    m_db1TblSchema = {};                        \
    m_db2TblSchema = {};                        \
    m_db1TblIndices = {}, m_db2TblIndices = {}; \
    m_db1TblTriggers = {}, m_db2TblTriggers = {};

#define CLEAR_MAPS            \
    m_db1TblSchema.clear();   \
    m_db2TblSchema.clear();   \
    m_db1TblIndices.clear();  \
    m_db2TblIndices.clear();  \
    m_db1TblTriggers.clear(); \
    m_db2TblTriggers.clear();

#define CHECK_INITIALIZED(x, message, r) \
    if ((x))                             \
    {                                    \
        cout << message                  \
             << endl;                    \
        return r;                        \
    }

namespace Kaco
{
    static map<string, string> initTablesSchema(const shared_ptr<IDbReader> &db, const vector<string> &tables)
    {
        map<string, string> result{};

        for (auto table : tables)
        {
            auto tableSchema = db->getTableSchema(table);
            stringstream ss;
            for (auto i = 0; i < tableSchema.size(); i++)
            {
                if (i)
                    ss << STR_SEPERATOR;
                ss << tableSchema[i];
            }
            result.insert({table, ss.str()});
        }

        return result;
    }

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

    static map<string, string> initTableTriggers(const shared_ptr<IDbReader> &db, const vector<string> &tables)
    {
        map<string, string> result{};

        for (auto table : tables)
        {
            auto tableTriggers = db->getTriggers(table);
            stringstream ss;
            for (auto i = 0; i < tableTriggers.size(); i++)
            {
                if (i)
                    ss << STR_SEPERATOR;
                ss << tableTriggers[i].first << VAL_SEPERATOR << tableTriggers[i].second;
            }
            result.insert({table, ss.str()});
        }

        return result;
    }

    DbCompare::DbCompare() : m_db1(nullptr), m_db2(nullptr), m_initialized(false)
    {
        INIT_VECTORS;
        INIT_MAPS;
    }

    DbCompare::DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2) : m_db1(db1), m_db2(db2), m_initialized(false)
    {
        INIT_VECTORS;
        INIT_MAPS;
    }

    DbCompare::~DbCompare()
    {
        CLEAR_MAPS;
    }

    bool DbCompare::initialize()
    {
        CHECK_INITIALIZED(m_initialized, MSG_ALREADY_INIT, FALSE);

        initDbTables();
        initDbTableSchema();
        initDbTableIndices();
        initDbTableTriggers();

        m_initialized = true;
        return m_initialized;
    }

    string DbCompare::compareAndMerge()
    {
        CHECK_INITIALIZED(!m_initialized, MSG_NOT_INIT, STR_NULL);

        string result{};
        // db1: source -> db2: target
        for (auto tblName_schema : m_db1TblSchema)
        {
            auto srcTblName = tblName_schema.first;
            // bool targetTblFound = (m_db2TblSchema.find(srcTblName) != m_db2TblSchema.end());
            bool targetTblFound = false;
            auto targetTblSchema = m_db2TblSchema[srcTblName];
            if (targetTblSchema != "")
                targetTblFound = true;
            if (targetTblFound)
            {
                // check for schemas
                auto srcTblSchema = tblName_schema.second;
                auto targetTblSchema = m_db2TblSchema[srcTblName];
                if (srcTblSchema == targetTblSchema)
                {
                    // check for the indices
                    auto srcTblIndices = m_db1TblIndices[srcTblName];
                    auto targetTblIndices = m_db2TblIndices[srcTblName];
                    if (srcTblIndices == targetTblIndices)
                    {
                        // check for triggers
                        auto srcTblTriggers = m_db1TblTriggers[srcTblName];
                        auto targetTblTriggers = m_db2TblTriggers[srcTblName];
                        if (srcTblTriggers == targetTblTriggers)
                        {
                            // TODO: complete it
                            // check for data
                        }
                        else
                        {
                            // TODO: complete it
                            // different triggers
                            cout << srcTblName << " is different in triggers" << endl;
                        }
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

    void DbCompare::testTableSchema()
    {
        cout << "db1 all the table schemas: " << endl;
        for (auto str : m_db1TblSchema)
            cout << str.first << ": " << str.second << endl;
        cout << endl
             << "db2 all the table schemas: " << endl;
        for (auto str : m_db2TblSchema)
            cout << str.first << ": " << str.second << endl;
    }

    void DbCompare::testTableIndices()
    {
        cout << "db1 all the table indices: " << endl;
        for (auto str : m_db1TblIndices)
            cout << str.first << ": " << str.second << endl;
        cout << endl
             << "db2 all the table indices: " << endl;
        for (auto str : m_db2TblIndices)
            cout << str.first << ": " << str.second << endl;
    }

    void DbCompare::testTableTriggers()
    {
        cout << "db1 all the table triggers: " << endl;
        for (auto str : m_db1TblTriggers)
            cout << str.first << ": " << str.second << endl;
        cout << endl
             << "db2 all the table triggers: " << endl;
        for (auto str : m_db2TblTriggers)
            cout << str.first << ": " << str.second << endl;
    }

    void DbCompare::testTableTriggers(string tableName)
    {
        cout << "db1::" << tableName << " table triggers: " << endl;
        cout << m_db1TblTriggers[tableName] << endl;
        cout << endl
             << "db2::" << tableName << " table triggers: " << endl;
        cout << m_db2TblTriggers[tableName] << endl;
    }

    void DbCompare::initDbTables()
    {
        m_db1Tables = m_db1->getTables();
        m_db2Tables = m_db2->getTables();
    }

    void DbCompare::initDbTableSchema()
    {
        auto tblSchema1 = initTablesSchema(m_db1, m_db1Tables);
        m_db1TblSchema = std::move(tblSchema1);

        auto tblSchema2 = initTablesSchema(m_db2, m_db2Tables);
        m_db2TblSchema = std::move(tblSchema2);
    }

    void DbCompare::initDbTableIndices()
    {
        auto tableIndices1 = initTableIndices(m_db1, m_db1Tables);
        m_db1TblIndices = std::move(tableIndices1);

        auto tableIndices2 = initTableIndices(m_db2, m_db2Tables);
        m_db2TblIndices = std::move(tableIndices2);
    }

    void DbCompare::initDbTableTriggers()
    {
        auto tableTriggers1 = initTableTriggers(m_db1, m_db1Tables);
        m_db1TblTriggers = std::move(tableTriggers1);

        auto tableTriggers2 = initTableTriggers(m_db2, m_db2Tables);
        m_db2TblTriggers = std::move(tableTriggers2);
    }

} // namespace Kaco
