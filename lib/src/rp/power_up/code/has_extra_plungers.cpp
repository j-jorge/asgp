#include "rp/power_up/has_extra_plungers.hpp"

#include "rp/android/get_activity_method.hpp"

#include <SDL2/SDL_system.h>
#include <cassert>

#ifndef __ANDROID__

bool rp::has_extra_plungers()
{
  return false;
}

#else

bool rp::has_extra_plungers()
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );
  assert( !env->ExceptionCheck() );

  android::method_info method
    ( android::get_activity_method
      ( "hasExtraPlungers",  "()Z" ) );

  const jboolean result
    ( method.env->CallBooleanMethod( method.object, method.id ) );
  
  assert( !env->ExceptionCheck() );

  return result;
}

#endif
