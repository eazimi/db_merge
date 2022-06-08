#ifndef TRIGGER_BUILDER_HPP
#define TRIGGER_BUILDER_HPP

#include "dump_builder.h"
#include <sqlite3.h>
#include <sstream>

namespace Kaco
{
    class DumpTriggerBuilder : public DumpBuilderBase
    {
        using Self = DumpTriggerBuilder;

    public:
        DumpTriggerBuilder(Dump &dump) : DumpBuilderBase{dump} {}

        Self &dump_triggers(sqlite3 *db)
        {
            if (dump.stmt_table)
                sqlite3_finalize(dump.stmt_table);
            bool rc_bool = dump.prepare(db, 
                    "SELECT sql FROM sqlite_master WHERE type = 'trigger';", dump.stmt_table);
            if(!rc_bool)
                return *this;
            auto triggers = dump.db_triggers();
            dump.oss << triggers << "COMMIT;\n";
            return *this;
        }
    };
} // namespace Kaco

#endif