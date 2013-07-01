/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief This toggle pauses the game when activated.
 * \author Julien Jorge
 */
#ifndef __RP_PAUSE_GAME_HPP__
#define __RP_PAUSE_GAME_HPP__

#include "engine/base_item.hpp"
#include "engine/export.hpp"

#include "engine/item_brick/item_with_toggle.hpp"

namespace rp
{
  /**
   * \brief This toggle pauses the game when activated.
   * \author Julien Jorge
   */
  class pause_game:
    public bear::engine::item_with_toggle< bear::engine::base_item >
  {
    DECLARE_BASE_ITEM(pause_game);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::item_with_toggle< bear::engine::base_item > super;

  public:
    pause_game();
    
  private:
    void on_toggle_on( bear::engine::base_item* activator );

  private:
    /** \brief The name of the layer to show. */
    std::string m_layer_name;

  }; // class pause_game
} // namespace rp

#endif // __RP_PAUSE_GAME_HPP__
