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

#ifndef __RP_JAVA_ACTIVITY_HPP__
#define __RP_JAVA_ACTIVITY_HPP__

#include <string>

namespace rp
{
  /**
   * \brief The c++ equivalent of the Android Java class of the game.
   */
  class java_activity
  {
  public:
    void open_url( const std::string& url ) const;
    void show_home() const;

    std::string get_device_model_name() const;

  private:
    void native_call( std::string method_name ) const;

  }; // class java_activity

} // namespace rp

#endif // __RP_JAVA_ACTIVITY_HPP__

#endif // __ANDROID__
