#ifndef TRIGGER_H
#define TRIGGER_H

#include <memory>
#include "data_types.hpp"
#include "IDbReader.hpp"

namespace Kaco
{
    class Trigger
    {   
        private:
            shared_ptr<IDbReader> m_main_db, m_ref_db;
            MAP_STR_VPS2 m_mainTriggers, m_refTriggers;
            MAP_STR_VPS2 getDbTriggers(const shared_ptr<IDbReader> &db, const vector<string> &db_tbls);

        public:
            Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db);
            void initDbTriggers(const vector<string> &main_tbls, const vector<string> &ref_tbls);

    };
} // namespace Kaco

#endif