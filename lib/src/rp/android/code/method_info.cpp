/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#ifdef __ANDROID__

#include "rp/android/method_info.hpp"

rp::android::method_info::method_info
( JNIEnv* env, jobject object, jmethodID id )
  : env( env ),
    object( object ),
    id( id )
{

}

void rp::android::method_info::release()
{
  env->DeleteLocalRef( object );

  env = nullptr;
  object = nullptr;
}

#endif
