/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#ifdef __ANDROID__

#ifndef __RP_ANDROID_METHOD_INFO_HPP__
#define __RP_ANDROID_METHOD_INFO_HPP__

#include <jni.h>

namespace rp
{
  namespace android
  {
    class method_info
    {
    public:
      method_info( JNIEnv* env, jobject object, jmethodID id );

      void release();
      
    public:
      JNIEnv* env;
      jobject object;
      jmethodID id;
    };
  }
}

#endif

#endif
