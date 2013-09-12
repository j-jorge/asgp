/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::android_logger class.
 * \author Julien Jorge
 */
#include "android_logger.hpp"

#include <android/log.h>

/*----------------------------------------------------------------------------*/
/**
 * \brief Prints something in the log.
 */
void rp::android_logger::write( const std::string& str )
{
  m_output += str;

  const std::string::size_type pos( m_output.find_last_of( '\n' ) );

  if ( pos != std::string::npos )
    {
      std::string log_string( m_output.substr( 0, pos ) );
      m_output = m_output.substr( pos + 1 );

      __android_log_print( ANDROID_LOG_INFO, "ASGP", "%s", log_string.c_str() );
    }
} // android_logger::write()

/*----------------------------------------------------------------------------*/
/**
 * \brief Prints the buffered logs.
 */
void rp::android_logger::flush()
{
  __android_log_print( ANDROID_LOG_INFO, "ASGP", "%s", m_output.c_str() );
  m_output.clear();
} // android_logger::write()
