/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief This toggle shows the key layer when activated.
 * \author Julien Jorge
 */
#ifndef __RP_SHOW_KEY_LAYER_HPP__
#define __RP_SHOW_KEY_LAYER_HPP__

#include "engine/base_item.hpp"
#include "engine/export.hpp"

#include "engine/item_brick/item_with_toggle.hpp"

namespace rp
{
  /**
   * \brief This toggle shows the key layer when activated.
   * \author Julien Jorge
   */
  class show_key_layer:
    public bear::engine::item_with_toggle< bear::engine::base_item >
  {
    DECLARE_BASE_ITEM(show_key_layer);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::item_with_toggle< bear::engine::base_item > super;

  public:
    show_key_layer();
    
  private:
    void on_toggle_on( bear::engine::base_item* activator );

  private:
    /** \brief The name of the layer to show. */
    std::string m_layer_name;

  }; // class show_key_layer
} // namespace rp

#endif // __RP_SHOW_KEY_LAYER_HPP__
