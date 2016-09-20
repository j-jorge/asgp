/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#ifdef __ANDROID__

#ifndef __RP_ANDROID_GET_ACTIVITY_METHOD_HPP__
#define __RP_ANDROID_GET_ACTIVITY_METHOD_HPP__

#include "rp/android/method_info.hpp"

namespace rp
{
  namespace android
  {
    method_info get_activity_method( const char* name, const char* signature );
  }
}

#endif

#endif
