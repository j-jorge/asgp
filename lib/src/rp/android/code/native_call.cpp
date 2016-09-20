#include "rp/android/native_call.hpp"

#include "rp/android/jni_call_bridge.hpp"

#include <jni.h>

namespace rp
{
  namespace android
  {
    static jni_call_bridge g_jni_call_bridge;
  }
}

rp::android::native_call_handle
rp::android::register_native_callback( const boost::function< void() >& f )
{
  return g_jni_call_bridge.register_callback( f );
}

extern "C" void Java_com_stuffomatic_jni_NativeCallBridge_call
( JNIEnv* env, jlong callback )
{
  rp::android::g_jni_call_bridge.call( callback );
}
