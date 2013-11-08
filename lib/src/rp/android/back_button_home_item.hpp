/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The back_button_home_item item simulates the home button when the back
 *        button is pressed on Android.
 */
#ifndef __RP_BACK_BUTTON_HOME_ITEM_HPP__
#define __RP_BACK_BUTTON_HOME_ITEM_HPP__

#include "engine/item_brick/item_with_input_listener.hpp"
#include "engine/base_item.hpp"

#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The back_button_home_item item simulates the home button when the
   *        back button is pressed on Android.
   */
  class back_button_home_item:
    public bear::engine::item_with_input_listener< bear::engine::base_item >
  {
    DECLARE_BASE_ITEM(back_button_home_item);

  private:
    /** \brief The type of the parent class. */
    typedef bear::engine::item_with_input_listener< bear::engine::base_item >
    super;

  public:
    void progress( bear::universe::time_type elapsed_time );

    bool key_released( const bear::input::key_info& key );

  }; // class back_button_home_item

} // namespace rp

#endif // __RP_BACK_BUTTON_HOME_ITEM_HPP__
