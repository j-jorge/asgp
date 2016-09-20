#include "rp/callback_queue.hpp"

#include "rp/level_scheduler.hpp"

BASE_ITEM_EXPORT( callback_queue, rp )

rp::callback_queue::callback_queue()
{
  set_global( true );
}

void rp::callback_queue::progress( bear::universe::time_type elapsed_time )
{
  process_pending_callbacks();
}

