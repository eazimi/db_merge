#ifndef TABLE_BUILDER_HPP
#define TABLE_BUILDER_HPP

#include "dump_builder.h"

namespace Kaco
{
    class DumpTableBuilder : public DumpBuilderBase
    {
    public:
        DumpTableBuilder(Dump &dump) : DumpBuilderBase{dump} {}
    };

} // namespace Kaco

#endif