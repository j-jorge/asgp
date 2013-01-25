/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the boss_controller class.
 * \author Sebastien Angibaud
 */
#include "rp/boss_controller.hpp"
#include "rp/game_variables.hpp"
#include "rp/zeppelin.hpp"

#include "engine/item_brick/loader/activable_sides_loader.hpp"
#include "universe/collision_info.hpp"

BASE_ITEM_EXPORT( boss_controller, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::boss_controller::boss_controller()
: m_y_reference_item(NULL), m_item(NULL), m_current_item(NULL), 
  m_zeppelin_item(NULL), m_hit_bound(0),
  m_required_zone( bear::universe::zone::middle_left_zone )
{
  set_size(50,1000);
} // rp::boss_controller::rp::boss_controller()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the value of a field of type item.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::boss_controller::set_item_field
( const std::string& name, bear::engine::base_item* value)
{
  bool result(true);

  if ( name == "boss_controller.y_reference_item" )
    m_y_reference_item = value;
  else if ( name == "boss_controller.item" )
    {
      m_item = value->clone();
      value->kill();
    }
  else if ( name == "boss_controller.zeppelin_item" )
    m_zeppelin_item = value;
  else
    result = super::set_item_field(name, value);

  return result;
} // boss_controller::set_item_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c unsigned \c integer.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::boss_controller::set_u_integer_field
( const std::string& name, unsigned int value )
{
  bool result(true);

  if ( name == "boss_controller.hit_bound" )
    m_hit_bound = value;
  else
    result = super::set_u_integer_field(name, value);

  return result;
} // boss_controller::set_u_integer_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c boolean.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::boss_controller::set_bool_field( const std::string& name, bool value )
{
  bool result(true);

  if ( name == "boss_controller.right_side" )
    m_required_zone = bear::universe::zone::middle_right_zone;
  else
    result = super::set_bool_field(name, value);

  return result;
} // boss_controller::set_bool_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss_controller::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  boss* b = dynamic_cast<boss*>(&that);

  if ( b != NULL )
    {  
      if ( m_y_reference_item != boss::handle_type(NULL) )
        b->set_y_reference( m_y_reference_item->get_top() );

      if ( ( info.get_collision_side() == m_required_zone ) && 
           ( m_item != NULL ) && ( m_current_item == boss::handle_type(NULL)) &&
           ( game_variables::get_boss_hits() >= m_hit_bound ) )
        m_current_item = b->set_drop_item(m_item,m_zeppelin_item);
    }
} // boss_controller::collision()

