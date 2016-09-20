#ifndef __RP_LEVEL_SCHEDULER_HPP__
#define __RP_LEVEL_SCHEDULER_HPP__

#include <boost/signals2/connection.hpp>
#include <boost/function.hpp>

namespace rp
{
  boost::signals2::connection
  schedule_call( const boost::function< void() >& f );

  void process_pending_callbacks();
}

#endif
