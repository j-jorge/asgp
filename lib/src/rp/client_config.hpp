#ifndef __RP_CLIENT_CONFIG__
#define __RP_CLIENT_CONFIG__

#include <rapidjson/document.h>

#include <boost/optional.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <array>
#include <chrono>

namespace rp
{
  class client_config
  {
  public:
    explicit client_config( const std::string& source );

    client_config( const client_config& ) = delete;
    client_config& operator=( const client_config& ) = delete;
    
    std::chrono::seconds get_minimum_ad_interval() const;
    std::size_t get_level_selection_interstitial_frequency() const;
    std::size_t get_restart_level_interstitial_frequency() const;

  private:
    void update_config( const std::string& config );
    
    template< std::size_t N >
    unsigned int get_u_int_or_default
    ( const std::array< const char*, N >& path, int fallback ) const;

    template< std::size_t N >
    boost::optional< const rapidjson::Document::ValueType& >
    find_config( const std::array< const char*, N >& path ) const;

  private:
    rapidjson::Document m_config;
    mutable boost::shared_mutex m_write_mutex;
    boost::signals2::connection m_update_connection;
  };

  const client_config& get_client_config();
}

#endif
