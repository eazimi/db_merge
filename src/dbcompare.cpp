#include "dbcompare.h"
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "dbcompare_funcs.hpp"
#include "global_funcs.hpp"
#include "data_types.hpp"

using namespace std;

// #define DEBUG
namespace Kaco
{
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

    string DbCompare::createNewTbl(std::string tblName)
    {
        string sql = "";

        // db1: target, db2: reference
        auto targetCtCmd = m_db1->getCreateTblCmd(tblName);
        auto refCtCmd = m_db2->getCreateTblCmd(tblName);

        print<vector<string>>("-> targetCtCmd", {targetCtCmd});
        print<vector<string>>("-> refCtCmd", {refCtCmd});

        // columns and constraints
        auto targetColsCons = splitCreateTblCmd(targetCtCmd);
        auto refColsCons = splitCreateTblCmd(refCtCmd);

        print<vector<string>>("-> targetColsCons", targetColsCons);
        print<vector<string>>("-> refColsCons", refColsCons);

        auto diffColsCons = getColsConsDiff(targetColsCons, refColsCons);
        auto diffTargetColsCons = diffColsCons.first;
        auto diffRefColsCons = diffColsCons.second;
        auto sharedColsCons = getColsConsIntersect(targetColsCons, refColsCons);

        print<vector<string>>("-> diffTargetColsCons", diffTargetColsCons);
        print<vector<string>>("-> diffRefColsCons", diffRefColsCons);
        print<vector<string>>("-> sharedColsCons", sharedColsCons);

        // TODO: update this part by paying attention to the value that have been read from json config file, 
        // for now it is considered as true
        bool keepColConst = true;

        stringstream ss_ct;
        string newTblName = tblName + "_tmp";
        ss_ct << "CREATE TABLE " << newTblName << " (";
        for (auto const &col : refColsCons)
            ss_ct << col << ", ";

        auto pairDiffTargetColsCons = getColsAndConstraints(diffTargetColsCons);
        auto umapDiffTargetCols = pairDiffTargetColsCons.first;
        auto diffTargetColNamesDetails = getColNamesDetails(umapDiffTargetCols);
        auto diffTargetColNames = diffTargetColNamesDetails.first;
        auto diffTargetColDetails = diffTargetColNamesDetails.second;
        auto diffTargetContraints = pairDiffTargetColsCons.second;

        // check for the columns which are in the target but not in ref
        int diffcols_size = diffTargetColNames.size();
        for (auto i = 0; i < diffcols_size; i++)
        {
            if (keepColConst)
                ss_ct << diffTargetColDetails[i] << ", ";
        }

        // check for the constraints which are in the target but not in ref
        int diffconst_size = diffTargetContraints.size();
        for (auto i = 0; i < diffconst_size; i++)
        {
            if(keepColConst)
                ss_ct << diffTargetContraints[i] << ", ";
        }

        ss_ct << ")";
        sql = ss_ct.str();
        auto pos = sql.find_last_of(",");
        sql.replace(pos, 2, "");
        
        // split columns and constraints 
        auto pairRefColsCons = getColsAndConstraints(refColsCons);
        auto umapRefCols = pairRefColsCons.first;
        auto refConstraints = pairRefColsCons.second;

        auto refColNames = getColNamesDetails(umapRefCols).first;
        auto detailedRefCols = updateColNames(refColNames, SCHEMA_REF, tblName);

        print<vector<string>>("-> detailedRefCols", detailedRefCols);

        auto detailedDiffTargetCols = updateColNames(diffTargetColNames, SCHEMA_MAIN, tblName);
        
        print<vector<string>>("-> detailedDiffTargetCols", detailedDiffTargetCols);
        
        stringstream ss_sel;
        ss_sel << "SELECT ";
        for (const auto &rec : detailedRefCols)
            ss_sel << rec << ", ";

        // check for the columns which are in the target but not in ref
        for (auto i = 0; i < diffcols_size; i++)
        {
            if (keepColConst)
                ss_sel << detailedDiffTargetCols[i] << ", ";
        }
        
        auto partialInsCmd = ss_sel.str();
        ss_sel.str("");
        auto virgool_pos = partialInsCmd.find_last_of(",");
        partialInsCmd.replace(virgool_pos, 1, "");

        ss_sel << partialInsCmd << "FROM "
               << "\"" << SCHEMA_REF << "\"" << "." << tblName << " ";

        if (!sharedColsCons.empty())
        {
            auto sharedCols = getColsAndConstraints(sharedColsCons).first;
            auto sharedColNames = getColNamesDetails(sharedCols).first;
            stringstream ss_shared_cols;
            for (auto col : sharedColNames)
            {
                ss_shared_cols << "\"" << SCHEMA_REF << "\"" << "." << tblName << "." 
                               << col << " = " 
                               << "\"" << SCHEMA_MAIN << "\"" << "." << tblName << "."
                               << col << " AND ";
            }
            auto join = ss_shared_cols.str();
            ss_shared_cols.str("");
            if(!sharedColNames.empty())
            {
                auto and_pos = join.find_last_of("AND");
                join.replace(and_pos - 3, 5, ""); // remove " AND "
            }

            ss_sel << "LEFT JOIN " << "\"" << SCHEMA_MAIN << "\"" << "." << tblName
                   << " ON " << join << ";";
        }
        else
        {
            auto sel = ss_sel.str();
            ss_sel.str("");         
            auto space_pos = sel.find_last_of(" ");
            sel.replace(space_pos, 1, "");
            ss_sel << sel << ";";
        }

        stringstream ss_ins;
        ss_ins << "INSERT INTO " << newTblName << " " << ss_sel.str();

        print<vector<string>>("-> [dbcompare] insert command", {ss_ins.str()});

        return sql;
    }

} // namespace Kaco
