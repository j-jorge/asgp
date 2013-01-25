/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::bonus class.
 * \author Julien Jorge
 */
#include "rp/bonus.hpp"
#include "rp/cart.hpp"
#include "rp/game_variables.hpp"

#include "universe/collision_info.hpp"
#include "engine/level_globals.hpp"

#include "universe/forced_movement/forced_join.hpp"
#include "universe/forced_movement/forced_sequence.hpp"

#include "generic_items/decorative_effect.hpp"
#include "generic_items/decorative_item.hpp"

#include <boost/bind.hpp>

#include <claw/tween/easing/easing_quad.hpp>
#include <claw/tween/single_tweener.hpp>

BASE_ITEM_EXPORT( bonus, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::bonus::bonus()
: m_type(unknown_type), m_bonus_is_given(false), m_cart(NULL)
{ 
  set_system_angle_as_visual_angle(true);
} // bonus::bonus()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the resources needed by this item.
 */
void rp::bonus::pre_cache()
{
  super::pre_cache();

  set_phantom(true);
  get_level_globals().load_model("model/bonus.cm");
  get_level_globals().load_sound("sound/bwow.ogg");
} // bonus::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::bonus::on_enters_layer()
{
  super::on_enters_layer();  

  set_model_actor( get_level_globals().get_model("model/bonus.cm") );
  if ( m_type == cart_element )
    {
      start_model_action("cart_element");
      init_mark("middle");
      init_mark("back");
      init_mark("front");
    }
  else if ( m_type == plunger )
    start_model_action("plunger");
} // bonus::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::bonus::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  if ( m_bonus_is_given )
    {
      m_tweener_x_position.update(elapsed_time);
      m_tweener_y_position.update(elapsed_time);
    }
} // bonus::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type "string".
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::bonus::set_string_field
( const std::string& name, const std::string& value )
{
  bool result = true;

  if ( name == "bonus.type" )
    m_type = type_from_string(value);
  else
    result = super::set_string_field( name, value );

  return result;
} // bonus::set_string_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the item is well initialized.
 */
bool rp::bonus::is_valid() const
{
  return ( m_type != unknown_type ) && super::is_valid();
} // bonus::is_valid()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get a bonus type from its string representation.
 * \param t The string containing the type of the bonus.
 */
rp::bonus::bonus_type
rp::bonus::type_from_string( const std::string& t )
{
  if ( t == "cart_element" ) return cart_element;
  if ( t == "plunger" ) return plunger;

  return unknown_type;
} // bonus::type_from_string()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get a string containing the name of a bonus type.
 * \param t The bonus type.
 */
std::string rp::bonus::type_to_string( bonus_type t )
{
  switch (t)
    {
    case plunger: return "plunger";
    case cart_element: return "cart_element";
    default: return "error: invalid type value";
    }
} // bonus::type_to_string()

/*----------------------------------------------------------------------------*/
/**
 * \brief Call collision_check_and_bounce().
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::bonus::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  if ( m_bonus_is_given )
    return;
  
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL )
    if ( c->get_current_action_name() != "dead" && 
         c->get_current_action_name() != "takeoff" )
      {
        m_bonus_is_given = true;
        give_bonus(c);
        set_angular_speed(-20);
        set_mass(1);
        m_cart = c;
        
        m_tweener_y_position.insert
          ( claw::tween::single_tweener
            ( 0.0, 300.0, 0.5, 
              boost::bind( &rp::bonus::on_y_position_change, this, _1 ), 
              &claw::tween::easing_quad::ease_out ) );
        m_tweener_y_position.insert
          ( claw::tween::single_tweener
            ( 300.0, 50.0, 0.5, 
              boost::bind( &rp::bonus::on_y_position_change, this, _1 ), 
              &claw::tween::easing_quad::ease_in ) );
        m_tweener_y_position.on_finished
          (boost::bind( &rp::bonus::kill, this ));
        
        m_tweener_x_position.insert
          ( claw::tween::single_tweener
            ( get_horizontal_middle() - m_cart->get_horizontal_middle(), 
              0.0, 1.0, 
              boost::bind( &rp::bonus::on_x_position_change, this, _1 ), 
              &claw::tween::easing_quad::ease_out ) );
      }
} // bonus::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give the bonus.
 * \param c The cart of the collision.
 */
void rp::bonus::give_bonus( cart* c )
{
  if ( m_type == cart_element )
    c->give_element();
  else if ( m_type == plunger )
    c->give_plunger_bonus();

  get_level_globals().play_sound("sound/bwow.ogg");
} // bonus::give_bonus()

/*----------------------------------------------------------------------------*/
/**
 * \brief Init mark in cart_element action.
 * \param name The name of the mark.
 */
void rp::bonus::init_mark( const std::string& name )
{
  bear::engine::model_mark_placement element;
    
  if ( get_mark_placement( name, element ) )
    set_global_substitute
      ( name,
        new bear::visual::animation
        ( get_level_globals().auto_sprite
          ( "gfx/cart-theme/"+ game_variables::get_level_theme()+".png", 
            name ) ) );
} // bonus::init_mark()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the y-coordinate changes.
 * \param value The new y-coordinate.
 */
void rp::bonus::on_y_position_change
(const bear::universe::coordinate_type& value)
{
  set_vertical_middle( m_cart->get_vertical_middle() + value );
} // bonus::on_y_position_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the x-coordinate changes.
 * \param value The new x-coordinate.
 */
void rp::bonus::on_x_position_change
(const bear::universe::coordinate_type& value)
{
  set_horizontal_middle( m_cart->get_horizontal_middle() + value);
} // bonus::on_x_position_change
