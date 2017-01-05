/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#include "rp/sharing/share.hpp"

#include "rp/android/get_activity_method.hpp"

#include <SDL2/SDL_system.h>

#include <claw/logger.hpp>

#ifndef __ANDROID__
void rp::share( const std::string& file_path, const std::string& message )
{
  claw::logger << claw::log_verbose << "Sharing file '" << file_path
               << "' with message '" << message << "'.\n";
}
#else

void rp::share( const std::string& file_path, const std::string& message )
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );
  assert( !env->ExceptionCheck() );

  android::method_info method
    ( android::get_activity_method( "share",  "(Ljava/lang/String;[B)V" ) );

  const std::size_t size( message.size() );
  const jbyteArray message_bytes( env->NewByteArray( size ) );
  env->SetByteArrayRegion
    ( message_bytes, 0, size,
      reinterpret_cast< const jbyte* >( message.c_str() ) );

  const jstring f( jstring( env->NewStringUTF( file_path.c_str() ) ) );
  method.env->CallVoidMethod( method.object, method.id, f, message_bytes );
  method.release();
  
  env->DeleteLocalRef( message_bytes );
  env->DeleteLocalRef( f );
  assert( !env->ExceptionCheck() );
}
#endif


