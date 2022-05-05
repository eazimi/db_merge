#include "trigger.h"

namespace Kaco
{
    Trigger::Trigger(shared_ptr<IDbReader> main_db, shared_ptr<IDbReader> ref_db) : m_main_db(main_db), m_ref_db(ref_db)
    {
    }

} // namespace Kaco