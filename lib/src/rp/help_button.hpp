/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a button to display help.
 * \author Sebastien Angibaud
 */
#ifndef __RP_HELP_BUTTON_HPP__
#define __RP_HELP_BUTTON_HPP__

#include "generic_items/mouse_detector.hpp"
#include "engine/export.hpp"
#include "engine/item_brick/basic_renderable_item.hpp"
#include "visual/sprite.hpp"

namespace rp
{
  /**
   * \brief The class describing a button to display help.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class help_button :
    public bear::engine::basic_renderable_item<bear::mouse_detector>
  {
    DECLARE_BASE_ITEM(help_button);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::basic_renderable_item< bear::mouse_detector > super;

  public:
    help_button();
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void get_visual
    ( std::list<bear::engine::scene_visual>& visuals ) const;

    bool mouse_released
    ( bear::input::mouse::mouse_code button,
      const claw::math::coordinate_2d<unsigned int>& pos );
    
  private:
    /** \brief The sprite of the item. */
    bear::visual::sprite m_sprite;  
  }; // class help_button
} // namespace rp

#endif // __RP_HELP_BUTTON_HPP__
