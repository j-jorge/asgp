/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#include "rp/message/level_capture_progress_message.hpp"

#include "rp/transition_effect/level_ending_effect.hpp"

rp::level_capture_progress_message::level_capture_progress_message( double p )
  : m_progress( p )
{

}

bool rp::level_capture_progress_message::apply_to( level_ending_effect& that )
{
  that.set_level_capture_progress( m_progress );

  return true;
}
