/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#include "rp/events/tag_level_event.hpp"

#include "rp/game_variables.hpp"
#include "rp/events/make_event_property.hpp"
#include "rp/events/tag_event.hpp"

#include <claw/logger.hpp>

void rp::tag_level_event( const std::string& type )
{
  tag_level_event( type, {} );
}

void rp::tag_level_event( const std::string& type, const event_properties& p )
{
  event_properties properties( p );
  properties.push_back
    ( make_event_property( "level", game_variables::get_level_number() ) );
  properties.push_back
    ( make_event_property( "serial", game_variables::get_serial_number() ) );
  
  tag_event( type, properties );
}

