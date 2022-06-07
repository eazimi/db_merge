#include "dump_builder.h"
#include "table_builder.hpp"
#include "trigger_builder.hpp"

namespace Kaco
{
    DumpTableBuilder DumpBuilderBase::tables() const
    {
        return DumpTableBuilder{dump};
    }

    DumpTriggerBuilder DumpBuilderBase::triggers() const
    {
        return DumpTriggerBuilder{dump};
    }
} // namespace Kaco