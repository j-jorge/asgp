/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The c++ equivalent of the Android Java class of the game.
 */
#ifdef __ANDROID__

#include "rp/android/java_activity.hpp"

#include <jni.h>
#include <SDL2/SDL_system.h>

#include <cassert>

/*----------------------------------------------------------------------------*/
/**
 * \brief Tag an event in the analytics service.
 * \param tag The event.
 */
void rp::java_activity::tag_event( const std::string& tag ) const
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );

  assert( !env->ExceptionCheck() );

  jobject activity( static_cast<jobject>( SDL_AndroidGetActivity() ) );
  jclass clazz( env->GetObjectClass(activity) );

  const jmethodID method_id
    ( env->GetMethodID( clazz, "tagEvent", "(Ljava/lang/String;)V" ) );

  jstring java_string_tag( (jstring)(env->NewStringUTF(tag.c_str()) ) );
  env->CallVoidMethod( activity, method_id, java_string_tag );
  env->DeleteLocalRef(java_string_tag);
    
  env->DeleteLocalRef(activity);

  assert( !env->ExceptionCheck() );
} // java_activity::tag_event()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tells the system to open a given URL.
 * \param url The URL to open.
 */
void rp::java_activity::open_url( const std::string& url ) const
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );

  assert( !env->ExceptionCheck() );

  jobject activity( static_cast<jobject>( SDL_AndroidGetActivity() ) );
  jclass clazz( env->GetObjectClass(activity) );

  const jmethodID method_id
    ( env->GetMethodID( clazz, "openUrl", "(Ljava/lang/String;)V" ) );

  jstring java_string_url( (jstring)(env->NewStringUTF(url.c_str()) ) );
  env->CallVoidMethod( activity, method_id, java_string_url );
  env->DeleteLocalRef(java_string_url);
    
  env->DeleteLocalRef(activity);

  assert( !env->ExceptionCheck() );
} // java_activity::open_url()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tells the system to show the home screen.
 */
void rp::java_activity::show_home() const
{
  native_call( "showHome" );
} // java_activity::show_home()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the model name of the device on which the game runs.
 */
std::string rp::java_activity::get_device_model_name() const
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );

  assert( !env->ExceptionCheck() );

  jobject activity( static_cast<jobject>( SDL_AndroidGetActivity() ) );
  jclass clazz( env->GetObjectClass(activity) );

  const jmethodID method_id
    ( env->GetMethodID( clazz, "getDeviceModelName", "()Ljava/lang/String;" ) );

  const jstring java_device_name
    ( (jstring)env->CallObjectMethod(activity, method_id) );
  const char* device_name( env->GetStringUTFChars( java_device_name, NULL ) );
  const std::string result(device_name);

  env->ReleaseStringUTFChars(java_device_name, device_name);
  env->DeleteLocalRef(activity);

  assert( !env->ExceptionCheck() );

  return result;
} // java_activity::get_device_model_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tells the system to show the ads.
 */
void rp::java_activity::show_ads() const
{
  native_call( "showAds" );
} // java_activity::show_ads()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tells the system to hide the ads.
 */
void rp::java_activity::hide_ads() const
{
  native_call( "hideAds" );
} // java_activity::hide_ads()

/*----------------------------------------------------------------------------*/
/**
 * \brief Calls a native void method with no arguments.
 * \param method_name The name of the method to call.
 */
void rp::java_activity::native_call( std::string method_name ) const
{
  JNIEnv * env( static_cast<JNIEnv*>( SDL_AndroidGetJNIEnv() ) );

  assert( !env->ExceptionCheck() );

  jobject activity( static_cast<jobject>( SDL_AndroidGetActivity() ) );
  jclass clazz( env->GetObjectClass(activity) );

  const jmethodID method_id
    ( env->GetMethodID( clazz, method_name.c_str(), "()V" ) );

  env->CallVoidMethod( activity, method_id );
    
  env->DeleteLocalRef(activity);

  assert( !env->ExceptionCheck() );
} // java_activity::native_call()

#endif // __ANDROID__
