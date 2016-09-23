/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#include "rp/events/tag_event.hpp"

#include "rp/android/get_activity_method.hpp"

#include <SDL2/SDL_system.h>

#include <claw/logger.hpp>

void rp::tag_event( const std::string& type )
{
  tag_event( type, {} );
}

#ifndef __ANDROID__
void rp::tag_event( const std::string& type, const event_properties& p )
{
  claw::logger << claw::log_verbose << "Event: " << type << '\n';

  for ( const auto& e : p )
    claw::logger << "  " << e.first << '=' << e.second << '\n';
}
#else

void rp::tag_event( const std::string& type, const event_properties& p )
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );
  assert( !env->ExceptionCheck() );

  const jclass c( env->FindClass( "java/util/HashMap" ) );
  const jmethodID constructor( env->GetMethodID( c, "<init>", "()V" ) );
  const jobject map( env->NewObject( c, constructor ) );
  
  const jmethodID put
        ( env->GetMethodID
          ( c, "put",
            "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;" ) );
  env->DeleteLocalRef( c );

  for ( const auto& e : p )
    {
      const jstring key( jstring( env->NewStringUTF( e.first.c_str() ) ) );
      const jstring value( jstring( env->NewStringUTF( e.second.c_str() ) ) );
      env->CallObjectMethod( map, put, key, value );
      assert( !env->ExceptionCheck() );
      env->DeleteLocalRef( value );
      env->DeleteLocalRef( key );
    }

  android::method_info tagger
    ( android::get_activity_method
      ( "tagEvent",  "(Ljava/lang/String;Ljava/util/Map;)V" ) );

  const jstring t( jstring( env->NewStringUTF( type.c_str() ) ) );
  tagger.env->CallVoidMethod( tagger.object, tagger.id, t, map );
  tagger.release();
  
  env->DeleteLocalRef( t );
  env->DeleteLocalRef( map );
  assert( !env->ExceptionCheck() );
}
#endif


