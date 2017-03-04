#include "rp/show_rate_dialog.hpp"

#include "rp/android/get_activity_method.hpp"

#include <SDL2/SDL_system.h>

#include <claw/logger.hpp>

#ifndef __ANDROID__
bool rp::show_rate_dialog()
{
  claw::logger << claw::log_verbose << "Show rate application dialog.\n";
  return false;
}
#else

bool rp::show_rate_dialog()
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );
  assert( !env->ExceptionCheck() );

  android::method_info method
    ( android::get_activity_method( "showRateAppDialog",  "()Z" ) );

  const jboolean result
    ( method.env->CallBooleanMethod( method.object, method.id ) );
  method.release();

  assert( !env->ExceptionCheck() );

  return result;
}
#endif

