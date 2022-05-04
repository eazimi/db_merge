#ifndef DBCOMPARE_DEFINES_HPP
#define DBCOMPARE_DEFINES_HPP

#define STR_SEPERATOR "##"
#define VAL_SEPERATOR "|"
#define STR_NULL std::string("")
#define FALSE false
#define SCHEMA_REF "ref"
#define SCHEMA_MAIN "main"
#define CT_REF "REFERENCES"
#define MSG_ALREADY_INIT "DbCompare is already initialized."
#define MSG_NOT_INIT "DbCompare is not initialized. Call initialize() first!"

#define CT_CONSTRAINT "CONSTRAINT"
#define CT_PRIMARY "PRIMARY"
#define CT_UNIQUE "UNIQUE"
#define CT_CHECK "CHECK"
#define CT_FOREIGN "FOREIGN"

#define INIT_VECTORS  \
    m_mainTbls = {}; \
    m_refTbls = {};

#define INIT_MAPS                               \
    m_mainTblSchema = {};                        \
    m_refTblSchema = {};                        \
    m_db1TblIndices = {}, m_db2TblIndices = {}; \
    m_mainTblTriggers = {}, m_refTblTriggers = {};

#define CLEAR_MAPS            \
    m_mainTblSchema.clear();   \
    m_refTblSchema.clear();   \
    m_db1TblIndices.clear();  \
    m_db2TblIndices.clear();  \
    m_mainTblTriggers.clear(); \
    m_refTblTriggers.clear();

#define CHECK_INITIALIZED(x, message, r) \
    if ((x))                             \
    {                                    \
        cout << message                  \
             << endl;                    \
        return r;                        \
    }

#define CHECK_IS_TBL_CONSTRAINT(x) \
    (((x) == CT_CONSTRAINT) ? true : (((x) == CT_PRIMARY) ? true : (((x) == CT_UNIQUE) ? true : (((x) == CT_CHECK) ? true : (((x) == CT_FOREIGN) ? true : false)))))

#endif