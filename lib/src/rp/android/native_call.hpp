#ifndef __RP_ANDROID_NATIVE_CALL_HPP__
#define __RP_ANDROID_NATIVE_CALL_HPP__

#include "rp/android/native_call_handle.hpp"

namespace rp
{
  namespace android
  {
    native_call_handle
    register_native_callback( const boost::function< void() >& f );
  }
}

#endif
