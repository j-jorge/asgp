/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#ifdef __ANDROID__

#include "rp/android/get_activity_method.hpp"

#include <SDL2/SDL_system.h>

#include <cassert>

rp::android::method_info
rp::android::get_activity_method( const char* name, const char* signature )
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );

  assert( !env->ExceptionCheck() );

  const jobject activity( static_cast<jobject>( SDL_AndroidGetActivity() ) );
  const jclass clazz( env->GetObjectClass(activity) );

  const method_info result
    ( env, activity, env->GetMethodID( clazz, name, signature ) );

  return result;
}

#endif
