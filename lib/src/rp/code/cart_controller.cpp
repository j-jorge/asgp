/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the cart_controller class.
 * \author Sebastien Angibaud
 */
#include "rp/cart_controller.hpp"

#include "rp/cart.hpp"

#include "engine/item_brick/loader/activable_sides_loader.hpp"

BASE_ITEM_EXPORT( cart_controller, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::cart_controller::cart_controller()
  : m_z_changed(false), m_z(0),
    m_right_orientation_changed(false), m_right_orientation(false),
    m_force_factor_changed(false), m_force_factor(0), m_is_checkpoint(false),
    m_can_jump_changed(false), m_can_jump(true),
    m_looping_changed(false), m_looping(true)
{
} // rp::cart_controller::rp::cart_controller()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type "real".
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::cart_controller::set_real_field
( const std::string& name, double value )
{
  bool ok = true;

  if (name == "cart_controller.z_position")
    {
      m_z_changed = true;
      m_z = value;
    }
  else if (name == "cart_controller.force_factor")
    {
      m_force_factor_changed = true;
      m_force_factor = value;
    }
  else
    ok = super::set_real_field(name, value);

  return ok;
} // cart_controller::set_real_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type "boolean".
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::cart_controller::set_bool_field
( const std::string& name, bool value )
{
  bool ok = true;

  if (name == "cart_controller.right_orientation")
    {
      m_right_orientation_changed = true;
      m_right_orientation = value;
    }
  else if (name == "cart_controller.checkpoint")
    m_is_checkpoint = true;
  else if (name == "cart_controller.can_jump")
    {
      m_can_jump_changed = true;
      m_can_jump = value;
    }
  else if (name == "cart_controller.looping")
    {
      m_looping_changed = true;
      m_looping = value;
    }
  else
    ok = super::set_bool_field(name, value);

  return ok;
} // cart_controller::set_bool_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void
rp::cart_controller::populate_loader_map( bear::engine::item_loader_map& m )
{
  super::populate_loader_map(m);

  m.insert( bear::engine::activable_sides_loader(*this) );
} // cart_controller::populate_loader_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::cart_controller::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  cart* p = dynamic_cast<cart*>(&that);

  if ( p != NULL )
    {  
      if ( satisfy_collision_condition(that) )
	{
	  if ( m_z_changed )
	    p->set_z_position(m_z);

	  if ( m_right_orientation_changed )
	    p->set_right_orientation(m_right_orientation);

	  if ( m_force_factor_changed )
	    p->set_force_factor(m_force_factor);

	  if ( m_is_checkpoint )
	    p->save_position();

	  if ( m_can_jump_changed )
	    p->set_can_jump(m_can_jump);

	  if ( m_looping_changed )
	    p->set_looping(m_looping);
	}
    }
  else
    super::collision(that,info);
} // cart_controller::collision()

