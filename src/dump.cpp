#include "dump.h"
#include "dump_builder.h"

namespace Kaco
{    
    DumpBuilder Dump::create()
    {
        return DumpBuilder{};
    }    
} // namespace Kaco