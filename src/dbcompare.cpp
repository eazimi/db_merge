#include "dbcompare.h"
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "dbcompare_funcs.hpp"

using namespace std;

// #define DEBUG
namespace Kaco
{
    DbCompare::DbCompare() : m_db1(nullptr), m_db2(nullptr), m_initialized(false)
    {
        m_trigger = make_shared<Trigger>(nullptr, nullptr);
        INIT_VECTORS;
        INIT_MAPS;
    }

    DbCompare::DbCompare(std::shared_ptr<IDbReader> db1, std::shared_ptr<IDbReader> db2) : m_db1(db1), m_db2(db2), m_initialized(false)
    {
        m_trigger = make_shared<Trigger>(db1, db2);
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
        m_trigger->initDbTriggers(m_mainTbls, m_refTbls);

        m_initialized = true;
        return m_initialized;
    }

    string DbCompare::compareAndMerge()
    {
        CHECK_INITIALIZED(!m_initialized, MSG_NOT_INIT, STR_NULL);

        string result{};
        // db1: source -> db2: target
        for (auto tblName_schema : m_mainTblSchema)
        {
            auto srcTblName = tblName_schema.first;
            // bool targetTblFound = (m_refTblSchema.find(srcTblName) != m_refTblSchema.end());
            bool targetTblFound = false;
            auto targetTblSchema = m_refTblSchema[srcTblName];
            if (targetTblSchema != "")
                targetTblFound = true;
            if (targetTblFound)
            {
                // check for schemas
                auto srcTblSchema = tblName_schema.second;
                auto targetTblSchema = m_refTblSchema[srcTblName];
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

    void DbCompare::testGetTables()
    {
        auto db1_tables = m_db1->getTables();
        auto db2_tables = m_db2->getTables();
        print<vector<string>>("-> tables in db1", db1_tables);
        print<vector<string>>("-> tables in db2", db2_tables);
    }

    void DbCompare::testTableSchema()
    {
        print(m_mainTblSchema, "-> db1 all the table schemas");
        print(m_refTblSchema, "-> db2 all the table schemas");
    }

    void DbCompare::testTableIndices()
    {
        print(m_db1TblIndices, "-> db1 all the table indices");
        print(m_db2TblIndices, "-> db2 all the table indices");
    }

    void DbCompare::testTableTriggers()
    {
        print(m_mainTblTriggers, "-> main db, all the table triggers 2", "main");
        print(m_refTblTriggers, "-> ref db, all the table triggers 2", "ref");
    }

    void DbCompare::testTableTriggers(string tableName)
    {
        auto triggers_main = m_mainTblTriggers[tableName];
        auto triggers_ref = m_refTblTriggers[tableName];
        stringstream ss;
        ss << "-> triggers of " << tableName << " table in the main db";
        print(triggers_main, ss.str(), "main", tableName);
        ss.str("");
        ss << "-> triggers of " << tableName << " table in the ref db";
        print(triggers_ref, ss.str(), "ref", tableName);
    }

    void DbCompare::testCreateNewTbl()
    {
        string sql;
        // sql = createNewTbl("inv");
        // sql = createNewTbl("countrySettingCfg");
        // sql = createNewTbl("accounts");
        sql = createNewTbl("invCfg");
        print<vector<string>>("-> create table", {sql});
    }

    void DbCompare::testDiffTableNames()
    {
        auto diff_tbls = diffTblNames();
        print<vector<string>>("-> table [names] in db1 but not in db2", diff_tbls.first);
        print<vector<string>>("-> table [names] in db2 but not in db1", diff_tbls.second);
    }

    void DbCompare::testDiffTableSchemas()
    {
        auto diff_schema = diffTblSchemas();
        print(diff_schema, "-> difference in schemas", "shcema in main db", "schema in ref db");
    }

    void DbCompare::testDiffTableTriggers()
    {
        auto diff_trigger = diffTblTriggers();
        print(diff_trigger.first, "-> trigger in the main db but not in the ref db", "main", false); 
        print(diff_trigger.second, "-> trigger in the ref db but not in the main db", "ref", false); 
    }

    void DbCompare::testDiffTableTriggers(string table_name)
    {
        auto diff_trigger = diffTblTriggers(table_name);
        stringstream ss;
        ss << "-> trigger in the main::" << table_name << " but not in the ref::" << table_name;
        print(diff_trigger.first, ss.str(), "main", table_name, false);
        ss.str("");
        ss << "-> trigger in the ref::" << table_name << " but not in the main::" << table_name;
        print(diff_trigger.second, ss.str(), "ref", table_name, false); 

        auto diff_m_trigger = m_trigger->diffTriggerSingleTbl(table_name);
        stringstream ss_trigger;
        ss_trigger << "-> [m_trigger] trigger in the main::" << table_name << " but not in the ref::" << table_name;
        print(diff_m_trigger.first, ss_trigger.str(), "main", table_name, false);
        ss_trigger.str("");
        ss_trigger << "-> [m_trigger] trigger in the ref::" << table_name << " but not in the main::" << table_name;
        print(diff_m_trigger.second, ss_trigger.str(), "ref", table_name, false); 
    }

    void DbCompare::initDbTables()
    {
        m_mainTbls = m_db1->getTables();
        m_refTbls = m_db2->getTables();
    }

    void DbCompare::initDbTableSchema()
    {
        auto tblSchema1 = initTablesSchema(m_db1, m_mainTbls);
        m_mainTblSchema = std::move(tblSchema1);

        auto tblSchema2 = initTablesSchema(m_db2, m_refTbls);
        m_refTblSchema = std::move(tblSchema2);
    }

    void DbCompare::initDbTableIndices()
    {
        auto tableIndices1 = initTableIndices(m_db1, m_mainTbls);
        m_db1TblIndices = std::move(tableIndices1);

        auto tableIndices2 = initTableIndices(m_db2, m_refTbls);
        m_db2TblIndices = std::move(tableIndices2);
    }

    void DbCompare::initDbTableTriggers()
    {
        auto tbl_triggers = initTblTriggers(m_db1, m_mainTbls);
        m_mainTblTriggers = std::move(tbl_triggers);

        tbl_triggers = initTblTriggers(m_db2, m_refTbls);
        m_refTblTriggers = std::move(tbl_triggers);
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

        print<vector<string>>("-> insert command", {ss_ins.str()});

        return sql;
    }

    pair<vector<string>, vector<string>> DbCompare::diffTblNames()
    {        
        auto diff_tbls = getDiff(m_mainTbls, m_refTbls);

        // first: tables in the main db but not in the ref db
        // second: tables in the ref db but not in the main db
        return {diff_tbls.first, diff_tbls.second};
    }

    // 0: table name, 1: table schema in main db, 2: table schema in ref db
    // returns the result of schema comparison between common tables 
    vector<TUPLE_ALL_STR> DbCompare::diffTblSchemas() 
    {        
        vector<TUPLE_ALL_STR> diff_schema = {};
        auto common_tbls = getIntersect(m_mainTbls, m_refTbls);
        for (auto str : common_tbls)
        {
            auto mainTblSchema = m_mainTblSchema[str];
            auto refTblSchema = m_refTblSchema[str];
            bool isEqual = (mainTblSchema == refTblSchema);
            if (!isEqual)
            {
                auto diff = make_tuple(str, mainTblSchema, refTblSchema);
                diff_schema.push_back(std::move(diff));
            }
        }
        return diff_schema;
    }

    // returns diff in triggers in the form of a pair
    // first: triggers in the main db but not in the ref db
    // second: triggers in the ref db but not in the main db
    // MAP_STR_VPS2: map<tbl_name, vector<pair<trigger_name, trigger_sql>>>
    PA_MAP_SVPS2 DbCompare::diffTblTriggers()     
    {
        PA_MAP_SVPS2 diff_triggers = {};
        MAP_STR_VPS2 diff_triggers_main, diff_triggers_ref;
        auto common_tbls = getIntersect(m_mainTbls, m_refTbls);
        for (auto str : common_tbls)
        {
            auto main_trigger_vec = diffTblTriggers(str).first;
            if (!main_trigger_vec.empty())
                diff_triggers_main.insert({str, main_trigger_vec});

            auto ref_trigger_vec = diffTblTriggers(str).second;
            if (!ref_trigger_vec.empty())
                diff_triggers_ref.insert({str, ref_trigger_vec});
        }
        return {diff_triggers_main, diff_triggers_ref};
    }

    // returns triggers diff for a particular table
    // returns pair<vector<pair<trigger_name, trigger_sql>>, vector<pair<trigger_name, trigger_sql>>>
    // first: triggers which are in the main::table but not in the ref::table
    // second: triggers which are in the ref::table but not in the main::table
    PA_VEC_PS2 DbCompare::diffTblTriggers(string table_name)
    {
        VEC_PS2 diff_triggers_main = {}, diff_triggers_ref = {}; // vector<pair<trigger_name, trigger_sql>>

        auto mainTblTriggers = m_mainTblTriggers[table_name];
        auto refTblTriggers = m_refTblTriggers[table_name];
        vector<string> vec_main_formatted_triggers, vec_ref_formatted_triggers;

        // tuple<trigger_name, trigger_sql, formatted_trigger_sql>
        // get the formatted trigger in the second parameter of formatTriggers()
        vector<tuple<string, string, string>> mainTblTriggers_formatted = formatTriggers(mainTblTriggers, vec_main_formatted_triggers);
        vector<tuple<string, string, string>> refTblTriggers_formatted = formatTriggers(refTblTriggers, vec_ref_formatted_triggers);

        auto diff = getDiff(vec_main_formatted_triggers, vec_ref_formatted_triggers);

        if (!diff.first.empty() && !mainTblTriggers_formatted.empty())
            diff_triggers_main = retrieveTriggers(mainTblTriggers_formatted, diff.first);
        if (!diff.second.empty() && !refTblTriggers_formatted.empty())
            diff_triggers_ref = retrieveTriggers(refTblTriggers_formatted, diff.second);

        return make_pair(diff_triggers_main, diff_triggers_ref);
    }

} // namespace Kaco
