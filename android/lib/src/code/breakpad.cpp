/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#include <jni.h>

#include <client/linux/handler/exception_handler.h>
#include <client/linux/handler/minidump_descriptor.h>

#include <claw/logger.hpp>

static google_breakpad::ExceptionHandler* exception_handler;

bool dump_callback
( const google_breakpad::MinidumpDescriptor& descriptor, void* context,
  bool succeeded )
{
  claw::logger << claw::log_verbose << "Dump crash at " << descriptor.path()
               << std::endl;
  return succeeded;
}

extern "C"
{
  void Java_com_stuffomatic_asgp_ASGP_setUpBreakpad
  ( JNIEnv* env, jobject obj, jstring filepath )
  {
    const char* const path( env->GetStringUTFChars( filepath, 0 ) );
    google_breakpad::MinidumpDescriptor descriptor( path );

    exception_handler =
      new google_breakpad::ExceptionHandler
      ( descriptor, nullptr, &dump_callback, nullptr, true, -1 );
  }
}
