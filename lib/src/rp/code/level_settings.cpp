/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::level_settings class.
 * \author Sebastien Angibaud
 */
#include "rp/level_settings.hpp"
#include "rp/add_ingame_layers.hpp"
#include "rp/cart.hpp"
#include "rp/game_variables.hpp"
#include "rp/power_up/has_extra_plungers.hpp"

#include "engine/level.hpp"
#include "generic_items/timer.hpp"

BASE_ITEM_EXPORT( level_settings, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Contructor.
 */
rp::level_settings::level_settings()
: m_cart(NULL), m_add_ingame_layers(NULL),
  m_level_duration(0), m_level_info(""), m_level_theme("western"),
  m_boss_level(false), m_boss_transition(false)
{
} // level_settings::level_settings()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the item.
 */
void rp::level_settings::build()
{
  super::build();

  game_variables::load_variables(get_level().get_name());
  game_variables::set_bad_plunger_number(0);
  game_variables::set_bad_cannonball_number(0);
  game_variables::set_bad_balloon_number(0);
  game_variables::set_score(0);
  game_variables::set_balloons_number(0);
  game_variables::set_level_info(m_level_info);
  game_variables::set_level_theme(m_level_theme);
  game_variables::set_level_starting(m_add_ingame_layers == NULL);
  game_variables::set_cannonball_activation(true);
  game_variables::set_cannonball_validity(true);  
  game_variables::set_plunger_activation(true);
  game_variables::set_plunger_validity(true);
  game_variables::set_cart_elements_number(1,3);
  game_variables::set_cart_elements_number(2,3);
  game_variables::set_boss_hits(0);
  game_variables::set_boss_level(m_boss_level);
  game_variables::set_boss_transition(m_boss_transition);
  game_variables::set_ending_effect(false);
  game_variables::set_new_score(false);
  game_variables::set_last_combo( 0 );

  const int plunger_count( 3 + 3 * has_extra_plungers() );
  game_variables::set_plunger_total_number( plunger_count );
  game_variables::set_plunger_number( plunger_count );

  if ( ! m_boss_transition )
    {
      game_variables::set_last_medal(0);
      game_variables::set_level_ending(false);
    }

  kill();
} // level_settings::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::level_settings::on_enters_layer()
{
  super::on_enters_layer();

  if ( (m_cart != NULL) || (m_add_ingame_layers != NULL) )
    {
      bear::timer* item = new bear::timer(m_level_duration,true);
      new_item(*item);
      item->toggle_on(NULL);
  
      if ( m_cart != NULL )
        m_cart->set_level_timer(item);

      if ( m_add_ingame_layers != NULL )
        m_add_ingame_layers->set_level_timer(item);
    }
} // rp::level_settings::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c real.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::level_settings::set_real_field( const std::string& name, double value )
{ 
  bool result = true;

  if (name == "level_settings.level_duration")
    m_level_duration = value;
  else
    result = super::set_real_field(name, value);

  return result;
} // level_settings::set_real_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c boolean.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::level_settings::set_bool_field( const std::string& name, bool value )
{
  bool result(true);

  if ( name == "level_settings.is_boss_level" )
    m_boss_level = value;
  else if ( name == "level_settings.is_boss_transition" )
    m_boss_transition = value;
  else
    result = super::set_bool_field(name, value);

  return result;
} // level_settings::set_bool_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type "std::string".
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::level_settings::set_string_field
( const std::string& name, const std::string& value )
{
  bool result(true);

  if ( name == "level_settings.level_info" )
    m_level_info = value;
  else if ( name == "level_settings.level_theme" )
    m_level_theme = value;
  else
    result = super::set_string_field( name, value );

  return result;
} // level_settings::set_string_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c item.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::level_settings::set_item_field
( const std::string& name, bear::engine::base_item* value )
{
  bool result(true);

  if ( name == "level_settings.cart" )
    {
      m_cart = dynamic_cast<cart*>(value);

      if ( m_cart == NULL )
        claw::logger << claw::log_error << "level_settings::set_item_field:"
                     << " item is not an instance of 'rp::cart'."
                     << std::endl;
    }
  else if ( name == "level_settings.add_ingame_layers" )
    {
      m_add_ingame_layers = dynamic_cast<add_ingame_layers*>(value);

      if ( m_add_ingame_layers == NULL )
        claw::logger << claw::log_error << "level_settings::set_item_field:"
                     << " item is not an instance of 'rp::add_ingame_layers'."
                     << std::endl;
    }
  else
    result = super::set_item_field(name, value);

  return result;
} // level_settings::set_item_field()
