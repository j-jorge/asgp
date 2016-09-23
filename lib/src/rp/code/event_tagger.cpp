/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#include "rp/event_tagger.hpp"

#include "rp/events/event_properties.hpp"
#include "rp/events/make_event_property.hpp"
#include "rp/events/tag_event.hpp"
#include "rp/events/tag_level_event.hpp"

BASE_ITEM_EXPORT( event_tagger, rp )

rp::event_tagger::event_tagger()
    : m_tag_level_info( false )
{

}

bool rp::event_tagger::set_bool_field( const std::string& name, bool value )
{
  bool result( true );
  
  if ( name == "event_tagger.tag_level_info" )
    m_tag_level_info = value;
  else
    result = super::set_bool_field( name, value );

  return result;
}

bool rp::event_tagger::set_string_field
( const std::string& name, const std::string& value )
{
  bool result( true );
  
  if ( name == "event_tagger.type" )
    m_type = value;
  else
    result = super::set_string_field( name, value );

  return result;
}

bool rp::event_tagger::set_string_list_field
( const std::string& name, const std::vector< std::string >& value )
{
  bool result( true );
  
  if ( name == "event_tagger.properties" )
    {
      m_properties = value;
      result = ( value.size() % 2 ) == 0;
    }
  else
    result = super::set_string_list_field( name, value );

  return result;
}

void rp::event_tagger::on_toggle_on( bear::engine::base_item* activator )
{
  tag( true );
}

void rp::event_tagger::on_toggle_off( bear::engine::base_item* activator )
{
  tag( false );
}

void rp::event_tagger::tag( bool value ) const
{
  const std::size_t count( m_properties.size() );
  event_properties p;
  p.reserve( count / 2 + 1 );

  for ( std::size_t i( 0 ); i != count; i += 2 )
    p.emplace_back( m_properties[ i ], m_properties[ i + 1 ] );

  p.push_back( make_event_property( "state", value ? "on" : "off" ) );

  if ( m_tag_level_info )
    tag_level_event( m_type, p );
  else
    tag_event( m_type, p );
}

