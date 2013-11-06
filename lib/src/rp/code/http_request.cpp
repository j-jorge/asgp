/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::http_request class.
 */
#include "rp/http_request.hpp"

#include "rp/version.hpp"

#include <claw/socket_stream.hpp>
#include <claw/string_algorithm.hpp>

#include <boost/thread.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief 
 */
boost::signals::connection
rp::http_request::prepare_request
( const std::string& page, result_function on_result )
{
  boost::shared_ptr<http_request> request( new http_request(page) );
  boost::signals::connection result( request->m_on_result.connect(on_result) );
  boost::thread t( boost::bind( &http_request::get_page, request ) );

  return result;
} // http_request::prepare_request()

/*----------------------------------------------------------------------------*/
/**
 * \brief 
 */
void rp::http_request::get_page() const
{
  const std::string server( "www.stuff-o-matic.com" );
  const int port( 80 );

  claw::net::socket_stream server_connection( "www.stuff-o-matic.com", port );

  if ( !server_connection )
    {
#ifdef _DEBUG
      claw::logger << claw::log_verbose << "Cannot connect to " << server
                   << " on port " << port << "." << std::endl;
#endif
      return;
    }

  server_connection
    << "GET " << m_page << " HTTP/1.1\n"
    << "Host: www.stuff-o-matic.com\n"
    << "User-Agent: " << "ASGP," /*<< util::get_system_name() << ',' */<< RP_VERSION_STRING << "\n"
    << "Connection: Close\n"
    << '\n'
    << std::flush;

  std::string protocol;
  int code;
  std::string line;

  if ( server_connection >> protocol >> code )
    if ( claw::text::getline( server_connection, line ) )
      {
#ifdef _DEBUG
        claw::logger << claw::log_verbose <<  "Stats server response: " << code
                     << ' ' << line << std::endl;
#endif
        if ( code == 200 /* OK */ )
          {
            // Find the first empty line that separates the header and the body.
            while ( !line.empty() )
              claw::text::getline( server_connection, line );

            std::istreambuf_iterator<char> eos;

            const std::string result
              ( std::istreambuf_iterator<char>( server_connection ), eos );

            m_on_result( result );
          }
      }
} // http_request::request()

/*----------------------------------------------------------------------------*/
/**
 * \brief 
 */
rp::http_request::http_request( const std::string& page )
  : m_page( page )
{

} // http_request::http_request()
