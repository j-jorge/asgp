/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#ifndef RP_TAG_LEVEL_EVENT_H
#define RP_TAG_LEVEL_EVENT_H

#include "rp/events/event_properties.hpp"

namespace rp
{
  void tag_level_event( const std::string& type );
  void tag_level_event( const std::string& type, const event_properties& p );
}

#endif
