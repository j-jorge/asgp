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

#include "rp/util.hpp"
#include "rp/version.hpp"

#include <claw/socket_stream.hpp>
#include <claw/string_algorithm.hpp>

#include <boost/thread.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Requests a page from the server.
 * \param page The path to the page, without the server's name
 *        (e.g. "/path/to/file.php?param=0&value=1").
 * \param on_result The function called with the result sent by the server. The
 *        result contains the content of the page only.
 * \return The connection to which on_result is linked. Call disconnect() on the
 *         result if you do not need to be notified about the result.
 */
boost::signals2::connection
rp::http_request::request( const std::string& page, result_function on_result )
{
  boost::shared_ptr<http_request> request( new http_request(page) );
  boost::signals2::connection result( request->m_on_result.connect(on_result) );
  boost::thread t( boost::bind( &http_request::get_page, request ) );

  return result;
} // http_request::request()

/*----------------------------------------------------------------------------*/
/**
 * \brief Executes the request to the server.
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

  send_request( server_connection );
  parse_result( server_connection );
} // http_request::get_page()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sends the request to the server.
 * \param server_connection The stream in which we write the request.
 */
void rp::http_request::send_request( std::ostream& server_connection ) const
{
  server_connection
    << "GET " << get_encoded_url() << " HTTP/1.1\n"
    << "Host: www.stuff-o-matic.com\n"
    << "User-Agent: " << "ASGP," << util::get_system_name() << ','
    << RP_VERSION_STRING << "\n"
    << "Connection: Close\n"
    << '\n'
    << std::flush;
} // http_request::send_request()

/*----------------------------------------------------------------------------*/
/**
 * \brief Parses the result sent by the server.
 * \param server_connection The stream from which we read the result.
 */
void rp::http_request::parse_result( std::istream& server_connection ) const
{
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
} // http_request::parse_result()

/*----------------------------------------------------------------------------*/
/**
 * \brief Encode the url to be sent to the server.
 */
std::string rp::http_request::get_encoded_url() const
{
  std::ostringstream result;
  const std::string to_encode( " \"%-.<>\\^_`{|}~" );

  for ( std::string::const_iterator it( m_page.begin() );
        it != m_page.end(); ++it )
    if ( to_encode.find_first_of(*it) == std::string::npos )
      result << *it;
    else
      result << '%' << std::hex << (unsigned int)*it;

  return result.str();
} // http_request::get_encoded_url()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructs the request for a given page.
 * \param page The page to request to the server.
 */
rp::http_request::http_request( const std::string& page )
  : m_page( page )
{

} // http_request::http_request()
