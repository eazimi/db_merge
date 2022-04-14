#include "dbcompare.h"
#include <iostream>
#include <sstream>

using namespace std;

#define STR_SEPERATOR "##" 
#define CHECK_INITIALIZED(x) \
    if ((x)) { \
        cout << "DbCompare is already initialized." \
             << endl; \
        return false; \
    }

namespace Kaco
{
    static map<string, string> initTablesSchema(const shared_ptr<IDbReader> &db)
    {
        map<string, string> result {};
        auto tables = db->getTables();

        for (auto table : tables)
        {
            auto tableSchema = db->getTableSchema(table);
            stringstream ss;
            for(auto i=0; i<tableSchema.size(); i++)
            {
                if(i)
                    ss << STR_SEPERATOR;
                ss << tableSchema[i];
            }   
            result.insert({table, ss.str()});
        }

        return result;
    }

    DbCompare::DbCompare() : m_db1(nullptr), m_db2(nullptr)
    {
        m_db1TblSchema = {};
        m_db2TblSchema = {};
        m_db1TblIndices = {};
        m_db2TblIndices = {};
        m_db1TblTriggers = {}; 
        m_db2TblTriggers = {};
        m_initialized = false;
    }

    DbCompare::DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2) : m_db1(db1), m_db2(db2)
    {
        m_db1TblSchema = {};
        m_db2TblSchema = {};
        m_db1TblIndices = {};
        m_db2TblIndices = {};
        m_db1TblTriggers = {}; 
        m_db2TblTriggers = {};
        m_initialized = false;
    }

    DbCompare::~DbCompare()
    {
        m_db1TblSchema.clear();
        m_db2TblSchema.clear();
        m_db1TblIndices.clear();
        m_db2TblIndices.clear();
        m_db1TblTriggers.clear();
        m_db2TblTriggers.clear();
    }

    void DbCompare::initDbTableSchema()
    {
        auto tblSchema1 = initTablesSchema(m_db1);
        m_db1TblSchema = std::move(tblSchema1);

        auto tblSchema2 = initTablesSchema(m_db2);
        m_db2TblSchema = std::move(tblSchema2);
    }

    bool DbCompare::initialize()
    {
        CHECK_INITIALIZED(m_initialized);

        initDbTableSchema();

        m_initialized = true;
        return m_initialized;
    }

    void DbCompare::testTableSchema()
    {
        cout << "db1 all the table schemas: " << endl;
        for(auto str:m_db1TblSchema)
            cout << str.first << ": " << str.second << endl; 
        cout << endl << "db2 all the table schemas: " << endl;
        for(auto str:m_db2TblSchema)
            cout << str.first << ": " << str.second << endl; 
    }

    // std::vector<std::string> DbCompare::compareSchema(std::string tableName)
    // {
    //     vector<string> result {};
    //     // auto tables1 = m_db1->getTables();
    //     // auto tables2 = m_db2->getTables();
    //     // result.insert(result.end(), tables1.begin(), tables1.end());
    //     // result.insert(result.end(), tables2.begin(), tables2.end());
    //     return result;
    // }

} // namespace Kaco
