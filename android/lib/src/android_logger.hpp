/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Log messages using Android's log system.
 * \author Julien Jorge
 */
#ifndef __RP_ANDROID_LOGGER_HPP__
#define __RP_ANDROID_LOGGER_HPP__

#include <claw/log_stream.hpp>
#include <claw/non_copyable.hpp>

namespace rp
{
  /**
   * \brief This class write log messages using Android's log system.
   * \author Julien Jorge
   */
  class android_logger
    : public claw::log_stream,
      public claw::pattern::non_copyable
  {
  public:
    virtual void write( const std::string& str );
    virtual void flush();

  private:
    /** \brief The strings writen in the log and not yet followed by a new
        line. */
    std::string m_output;

  }; // class file_logger
} // namespace rp

#endif // __RP_ANDROID_LOGGER_HPP__
