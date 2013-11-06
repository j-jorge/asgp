/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The http_request class requests a page on Stuffomatic's web server.
 * \author Julien Jorge
 */
#ifndef __RP_HTTP_REQUEST_HPP__
#define __RP_HTTP_REQUEST_HPP__

#include <boost/signals.hpp>
#include <boost/function.hpp>

namespace rp
{
  /**
   * \brief The http_request class requests a page on Stuffomatic's web server.
   * \author Julien Jorge
   */
  class http_request
  {
  public:
    /** \brief The type of the function to call when the page is ready. */
    typedef boost::function<void (std::string)> result_function;

  public:
    static boost::signals::connection prepare_request
    ( const std::string& page, result_function on_result );

  private:
    void get_page() const;

    explicit http_request( const std::string& page );

    // not implemented
    http_request( const http_request& that );
    http_request& operator=( http_request that );

  private:
    /** \brief The page to request on the server. */
    const std::string m_page;

    /** \brief The signal triggered when the result is ready. */
    boost::signal<void (std::string)> m_on_result;

  }; // class http_request

} // namespace rp

#endif // __RP_HTTP_REQUEST_HPP__
