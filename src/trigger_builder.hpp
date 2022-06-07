#ifndef TRIGGER_BUILDER_HPP
#define TRIGGER_BUILDER_HPP

#include "dump_builder.h"

namespace Kaco
{
   class DumpTriggerBuilder : public DumpBuilderBase
   {
   public:
       DumpTriggerBuilder(Dump &dump) : DumpBuilderBase{dump} {}
   };
} // namespace Kaco

#endif