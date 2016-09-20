#include "rp/android/jni_call_bridge.hpp"

#include "rp/level_scheduler.hpp"

#include <boost/bind.hpp>

rp::android::jni_call_bridge::jni_call_bridge()
  : _next_id( 1 )
{

}
    
rp::android::native_call_handle
rp::android::jni_call_bridge::register_callback
( const boost::function< void() >& f )
{
  const native_call_handle result
    ( _next_id, _signals[ _next_id ].connect( f ) );
  ++_next_id;

  return result;
}

void rp::android::jni_call_bridge::call( std::int64_t id )
{
  assert( _signals.find( id ) != _signals.end() );
  schedule_call( boost::bind( &jni_call_bridge::trigger_signal, this, id ) );
}

void rp::android::jni_call_bridge::trigger_signal( std::int64_t id )
{
  assert( _signals.find( id ) != _signals.end() );

  signal s;
  s.swap( _signals[ id ] );
  _signals.erase( id );
  s();
}

