/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#include "rp/message/level_capture_ready_message.hpp"

#include "rp/transition_effect/level_ending_effect.hpp"

rp::level_capture_ready_message::level_capture_ready_message
( const std::string& path, const bear::visual::sprite& sprite )
  : m_path( path ),
    m_sprite( sprite )
{

}

bool rp::level_capture_ready_message::apply_to( level_ending_effect& that )
{
  that.set_level_capture( m_path, m_sprite );

  return true;
}
