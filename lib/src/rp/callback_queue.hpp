#ifndef __RP_CALLBACK_QUEUE_HPP__
#define __RP_CALLBACK_QUEUE_HPP__

#include "engine/base_item.hpp"
#include "engine/export.hpp"

namespace rp
{
  class callback_queue:
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM( callback_queue );

  public:
    typedef bear::engine::base_item super;

  public:
    callback_queue();
    
    void progress( bear::universe::time_type elapsed_time );
  };
}

#endif
