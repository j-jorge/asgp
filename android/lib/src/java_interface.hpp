/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The C++ methods exposed to the Java part.
 * \author Julien Jorge
 */
#ifndef __RP_JAVA_INTERFACE_HPP__
#define __RP_JAVA_INTERFACE_HPP__

#include <jni.h>

extern "C"
{
  JNIEXPORT void JNICALL Java_com_stuffomatic_asgp_ASGP_init( JNIEnv* env );

} // extern "C"

#endif // __RP_JAVA_INTERFACE_HPP__
