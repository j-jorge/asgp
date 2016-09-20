#include "rp/ad/show_interstitial.hpp"

#include "rp/level_scheduler.hpp"
#include "rp/android/get_activity_method.hpp"
#include "rp/android/native_call.hpp"

#include <cassert>

boost::signals2::connection
rp::show_interstitial( const boost::function< void() >& callback )
{
#ifdef __ANDROID__
  android::method_info m
    ( android::get_activity_method( "showInterstitial", "(J)V" ) );

  const android::native_call_handle result
    ( android::register_native_callback( callback ) );
  m.env->CallVoidMethod( m.object, m.id, result.id );

  assert( !m.env->ExceptionCheck() );
  m.release();

  return result.connection;
#else

  return schedule_call( callback );
#endif
}
