/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief This toggle saves the configuration when activated.
 * \author Julien Jorge
 */
#ifndef __RP_CONFIG_SAVE_HPP__
#define __RP_CONFIG_SAVE_HPP__

#include "engine/base_item.hpp"
#include "engine/item_brick/item_with_toggle.hpp"

#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief This toggle saves the configuration when activated.
   *
   * \author Julien Jorge
   */
  class config_save:
    public bear::engine::item_with_toggle<bear::engine::base_item>
  {
    DECLARE_BASE_ITEM(config_save);

  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::item_with_toggle<bear::engine::base_item> super;

  protected:
    void on_toggle_on( bear::engine::base_item* activator );
    void on_toggle_off( bear::engine::base_item* activator );

  }; // class config_save
} // namespace rp

#endif // __RP_CONFIG_SAVE_HPP__
