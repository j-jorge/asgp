/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a cart_controller.
 * \author Sebastien Angibaud
 */
#ifndef __RP_CART_CONTROLLER_HPP__
#define __RP_CART_CONTROLLER_HPP__

#include "engine/base_item.hpp"
#include "engine/export.hpp"

#include "engine/item_brick/activable_sides.hpp"
#include "engine/item_brick/item_with_restricted_z_collision.hpp"

namespace rp
{
  class cart;

  /**
   * \brief The class describing a cart_controller.
   *²
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class cart_controller :
    public bear::engine::item_with_restricted_z_collision
  < bear::engine::base_item >,
    public bear::engine::activable_sides
  {
    DECLARE_BASE_ITEM(cart_controller);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::item_with_restricted_z_collision
    < bear::engine::base_item > super;

  public:
    cart_controller();

    bool set_real_field( const std::string& name, double value );
    bool set_bool_field( const std::string& name, bool value );

  protected:
    void populate_loader_map( bear::engine::item_loader_map& m );

    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

  private:
    /** \brief Indicates if cast's z-position modification must be applied. */
    bool m_z_changed;

    /** \brief Cart's z-position to give. */
    double m_z;

    /** \brief Indicates if we give cart's orientation. */
    bool m_right_orientation_changed;

    /** \brief Cast's orientation (right orientation if true). */
    double m_right_orientation;

    /** \brief Indicates cart's force factor in move state. */
    bool m_force_factor_changed;

    /** \brief Cart's force factor in move state. */
    double m_force_factor;

    /** \brief Indicates if the controller is a checkpoint. */
    bool m_is_checkpoint;

    /** \brief Indicates if can_jump option is active. */
    bool m_can_jump_changed;

    /** \brief Indicates if cart can jump. */
    double m_can_jump;

    /** \brief Indicates if looping option is active. */
    bool m_looping_changed;

    /** \brief Indicates if cart can jump. */
    bool m_looping;
  }; // class cart_controller
} // namespace rp

#endif // __RP_CART_CONTROLLER_HPP__
