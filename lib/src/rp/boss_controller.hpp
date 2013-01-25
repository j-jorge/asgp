/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a boss_controller.
 * \author Sebastien Angibaud
 */
#ifndef __RP_BOSS_CONTROLLER_HPP__
#define __RP_BOSS_CONTROLLER_HPP__

#include "rp/boss.hpp"
#include "engine/base_item.hpp"
#include "engine/export.hpp"
#include "engine/item_brick/activable_sides.hpp"
#include "engine/item_brick/item_with_restricted_z_collision.hpp"

#include "universe/zone.hpp"

namespace rp
{
  /**
   * \brief The class describing a boss_controller.
   *²
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class boss_controller :
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM(boss_controller);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::base_item super;

  public:
    boss_controller();

    bool set_item_field
    ( const std::string& name, bear::engine::base_item* value);
    bool set_u_integer_field( const std::string& name, unsigned int value );
    bool set_bool_field( const std::string& name, bool value );

  protected:
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

  private:
    /** \brief The reference item for y-coordinate. */
    boss::handle_type m_y_reference_item;

    /** \brief The item that is copied. */
    bear::engine::base_item* m_item;

    /** \brief The current item. */
    boss::handle_type m_current_item;

    /** \brief The item under the zeppelin. */
    bear::engine::base_item* m_zeppelin_item;

    /** \brief The bound under which we don't create the drop item. */
    unsigned int m_hit_bound;

    /** \brief The required zone for collision. */
    bear::universe::zone::position m_required_zone;
  }; // class boss_controller
} // namespace rp

#endif // __RP_BOSS_CONTROLLER_HPP__
