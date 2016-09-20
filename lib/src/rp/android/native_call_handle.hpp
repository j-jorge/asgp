#ifndef __RP_ANDROID_NATIVE_CALL_HANDLE_HPP__
#define __RP_ANDROID_NATIVE_CALL_HANDLE_HPP__

#include <boost/signals2/connection.hpp>

namespace rp
{
  namespace android
  {
    struct native_call_handle
    {
      native_call_handle( std::size_t id, boost::signals2::connection c )
        : id( id ),
          connection( c )
      {

      }
      
      std::int64_t id;
      boost::signals2::connection connection;
    };
  }
}
  
#endif

