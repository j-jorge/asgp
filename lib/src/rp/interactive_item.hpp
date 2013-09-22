/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief An item that display a star on a given interactive object.
 * \author Angibaud Sebastien
 */
#ifndef __RP_INTERACTIVE_ITEM_HPP__
#define __RP_INTERACTIVE_ITEM_HPP__

#include "generic_items/decorative_item.hpp"
#include "universe/derived_item_handle.hpp"
#include "engine/export.hpp"

#include <claw/tween/tweener_group.hpp>

namespace rp
{
  /**
   * \brief An item that display a star on a given interactive object.
   * \author Sebastien Angibaud
   */
  class interactive_item:
    public bear::decorative_item
  {
    DECLARE_BASE_ITEM(interactive_item);

  public:
    /** \brief The type of the parent class. */
    typedef bear::decorative_item super;

    /** \brief The type of an handle on a reference_item. */
    typedef
    bear::universe::derived_item_handle<bear::engine::base_item> handle_type;

  public:
    interactive_item();
    interactive_item
    ( bear::engine::base_item* item, double sprite_factor, double area_factor,
      bear::universe::position_type gap,
      const bear::visual::animation& help = bear::visual::animation() );

    void pre_cache();

    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void get_visual
    ( std::list<bear::engine::scene_visual>& visuals ) const;

    void activate();
    void deactivate();

    void on_cannonball_factor_change( double factor );
    bear::universe::coordinate_type compute_size() const;
    void update_item();

  private:
    void find_cursor();
    bool is_colliding_with_cursor() const;

  private:
    /** \brief The item to follow. */
    handle_type m_item;

    /** \brief The cursor that triggers the item. */
    handle_type m_cursor;

    /** \brief The sprite of background star. */
    bear::visual::sprite m_background_sprite;

    /** \brief The sprite of plunger star. */
    bear::visual::sprite m_plunger_sprite;

    /** \brief The sprite of cannon star. */
    bear::visual::sprite m_cannonball_sprite;

    /** \brief Indicates if the cursor is in the interactive item. */
    bool m_activated;

    /** \brief The factor of cannonball sprite. */
    double m_cannonball_factor;

    /** \brief The set of current tweeners. */
    claw::tween::tweener_group m_tweeners;

    /** \brief The angular speed. */
    double m_angular_speed;

    /** \brief The factor of sprites. */
    double m_sprite_factor;

    /** \brief The factor of required area for activation. */
    double m_area_factor;

    /** \brief The gap with the center of mass of the item. */
    bear::universe::position_type m_gap;

  }; // class interactive_item
} // namespace rp

#endif // __RP_INTERACTIVE_ITEM_HPP__
