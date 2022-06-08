#ifndef DUMP_BUILDER_H
#define DUMP_BUILDER_H

#include "dump.h"

namespace Kaco
{
    class DumpTableBuilder;
    class DumpTriggerBuilder;

    class DumpBuilderBase
    {
    protected:
        Dump &dump;
        explicit DumpBuilderBase(Dump &dump) : dump{dump} {}

    public:
        DumpTableBuilder tables() const;
        DumpTriggerBuilder triggers() const;
        operator Dump() const
        {
            return std::move(dump);
        }
    };

    class DumpBuilder : public DumpBuilderBase
    {
    private:
        Dump dump;

    public:
        DumpBuilder() : DumpBuilderBase{dump} {}
    };
} // namespace Kaco

#endif