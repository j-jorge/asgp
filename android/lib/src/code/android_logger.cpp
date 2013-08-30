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
  __android_log_print( ANDROID_LOG_INFO, "ASGP", "%s", str.c_str() );
} // android_logger::write()
