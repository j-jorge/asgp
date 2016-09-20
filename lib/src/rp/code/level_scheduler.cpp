#include "rp/level_scheduler.hpp"

#include <boost/signals2/signal.hpp>

#include <cassert>

namespace rp
{
  static boost::signals2::signal< void() > g_pending;
}

boost::signals2::connection
rp::schedule_call( const boost::function< void() >& f )
{
  return g_pending.connect( f );
}

void rp::process_pending_callbacks()
{
  boost::signals2::signal< void() > s;
  s.swap( g_pending );
  s();
}
