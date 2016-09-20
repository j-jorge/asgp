#ifndef __RP_ANDROID_JNI_CALL_BRIDGE_HPP__
#define __RP_ANDROID_JNI_CALL_BRIDGE_HPP__

#include "rp/android/native_call_handle.hpp"

#include <boost/signals2/signal.hpp>

#include <unordered_map>

namespace rp
{
  namespace android
  {
    class jni_call_bridge
    {
    public:
      jni_call_bridge();
      
      native_call_handle
      register_callback( const boost::function< void() >& f );

      void call( std::int64_t id );

    private:
      void trigger_signal( std::int64_t id );
      
    private:
      typedef boost::signals2::signal< void() > signal;
      
    private:
      std::int64_t _next_id;
      std::unordered_map< std::int64_t, signal > _signals;
    };
  }
}

#endif
