/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::back_button_home_item class.
 */
#include "rp/android/back_button_home_item.hpp"

#include "rp/android/java_activity.hpp"

BASE_ITEM_EXPORT( back_button_home_item, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Updates the state of the item.
 * \param elapsed_time The elapsed time since the last call.
 */
void rp::back_button_home_item::progress
( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );
  progress_input_reader( elapsed_time );
} // back_button_home_item::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Puts the application on the background if the back button is pressed
 *        on Android. Does nothing on other systems or other keys.
 * \param key The released key.
 */
bool rp::back_button_home_item::key_released
( const bear::input::key_info& key )
{
  if ( key.get_code() != bear::input::keyboard::kc_back )
    return false;

#ifdef __ANDROID__

  java_activity activity;
  activity.show_home();

  return true;

#else
  return false;
#endif
} // back_button_home_item::key_released()
