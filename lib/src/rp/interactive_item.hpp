/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
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
    (bear::engine::base_item* item, double sprite_factor, double area_factor,
     bool display_star, bear::universe::position_type gap);

    void pre_cache();

    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void get_visual
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

    void activate();
    void deactivate();

    void on_cannonball_factor_change( double factor );
    bear::universe::coordinate_type compute_size() const;
    void update_item();

  private:
     /** \brief The item to follow. */
    handle_type m_item;

    /** \brief The sprite of background star. */
    bear::visual::sprite m_background_sprite;

    /** \brief The sprite of plunger star. */
    bear::visual::sprite m_plunger_sprite;

    /** \brief The sprite of cannon star. */
    bear::visual::sprite m_cannonball_sprite;

    /** \brief Indicates if the cursor is in the interactive item. */
    bool m_activated;

    /** \brief Indicates if the cursor collides during the last iteration. */
    bool m_cursor_in_collision;

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

    /** \brief Indicates that we display always the star. */
    bool m_display_star;

    /** \brief The gap with the center of mass of the item. */
    bear::universe::position_type m_gap;
  }; // class interactive_item
} // namespace rp

#endif // __RP_INTERACTIVE_ITEM_HPP__
