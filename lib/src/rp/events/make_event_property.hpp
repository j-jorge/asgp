/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#ifndef RP_MAKE_EVENT_PROPERTY_H
#define RP_MAKE_EVENT_PROPERTY_H

#include "rp/events/event_properties.hpp"

#include <boost/lexical_cast.hpp>

namespace rp
{
  template< typename T >
  typename event_properties::value_type make_event_property
  ( const std::string& k, const T& v )
  {
    return
      event_properties::value_type
      ( k, boost::lexical_cast< std::string >( v ) );
  }
}

#endif
