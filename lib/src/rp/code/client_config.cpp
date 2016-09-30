#include "rp/client_config.hpp"

#include "rp/http_request.hpp"

#include <boost/thread/lock_types.hpp>

rp::client_config::client_config( const std::string& source )
  : m_update_connection
    ( http_request::request
      ( source, boost::bind( &client_config::update_config, this, _1 ) ) )
{

}

std::chrono::seconds rp::client_config::get_minimum_ad_interval() const
{
  return
    std::chrono::seconds
    ( get_u_int_or_default
      ( std::array< const char*, 2 >{ "ad", "minimum_interval.s" }, 1 ) );
}

std::size_t
rp::client_config::get_level_selection_interstitial_frequency() const
{
  return
    get_u_int_or_default
    ( std::array< const char*, 2 >
      { "ad", "level_selection_interstital_frequency" },
      1 );
}

std::size_t
rp::client_config::get_restart_level_interstitial_frequency() const
{
  return
    get_u_int_or_default
    ( std::array< const char*, 2 >
      { "ad", "restart_level_interstital_frequency" },
      2 );
}

void rp::client_config::update_config( const std::string& config )
{
  rapidjson::Document new_config;

  if ( new_config.Parse( config.c_str() ).HasParseError()
       || !new_config.IsObject() )
    return;
  
  const boost::lock_guard< boost::shared_mutex > lock( m_write_mutex );
  m_config.Swap( new_config );
}

template< std::size_t N >
unsigned int rp::client_config::get_u_int_or_default
( const std::array< const char*, N >& path, int fallback ) const
{
  const boost::optional< const rapidjson::Document::ValueType& > config
    ( find_config( path ) );

  if ( config && config->IsUint() )
    return config->GetUint();

  return fallback;
}

template< std::size_t N >
boost::optional< const rapidjson::Document::ValueType& >
rp::client_config::find_config( const std::array< const char*, N >& path ) const
{
  const boost::shared_lock< boost::shared_mutex > lock( m_write_mutex );

  const auto eit( m_config.MemberEnd() );
  std::size_t i( 0 );
  auto it( m_config.FindMember( path[ i ] ) );
  
  for ( ; ( i != N ) && ( it != eit ); ++i )
    if ( !it->value.IsObject() )
      return boost::none;
    else
      it = it->value.FindMember( path[ i ] );

  if ( it == eit )
    return boost::none;

  return it->value;
}

const rp::client_config& rp::get_client_config()
{
  static client_config result( "/asgp/business/client-config.json" );
  return result;
}
