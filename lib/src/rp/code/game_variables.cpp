/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::game_variables class.
 * \author Julien Jorge
 */
#include "rp/game_variables.hpp"
#include "rp/defines.hpp"

#include "engine/game.hpp"
#include "engine/variable/variable.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief A function that gets a game variable.
 * \param n The name of the variable.
 * \param def The default value to return if the variable is not set.
 */
template<typename T>
static T rp_game_variables_get_value( const std::string& n, const T& def )
{
  bear::engine::variable<T> var(n);

  if ( bear::engine::game::get_instance().game_variable_exists(var) )
    {
      bear::engine::game::get_instance().get_game_variable(var);
      return var.get_value();
    }
  else
    return def;
} // rp_game_variables_get_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of the current level.
 */
unsigned int rp::game_variables::get_level_number()
{
  return 
    rp_game_variables_get_value
    ( make_persistent_variable_name("scenario/level_number"), (unsigned int)0 );
} // game_variables::get_level_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of the level.
 * \param n The number of the level.
 */
void rp::game_variables::set_level_number( unsigned int n )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( make_persistent_variable_name("scenario/level_number"), n ) );
} // game_variables::set_level_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of the current serial.
 */
unsigned int rp::game_variables::get_serial_number()
{
  return 
    rp_game_variables_get_value
    ( make_persistent_variable_name("scenario/serial_number"), 
      (unsigned int)0 );
} // game_variables::get_serial_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of the serial.
 * \param n The number of the serial.
 */
void rp::game_variables::set_serial_number( unsigned int n )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( make_persistent_variable_name("scenario/serial_number"), n ) );
} // game_variables::set_serial_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the unlocked serial.
 */
unsigned int rp::game_variables::get_unlocked_serial()
{
  return 
    rp_game_variables_get_value
    ( make_persistent_variable_name("scenario/unlocked_serial"), 
      (unsigned int)0 );
} // game_variables::get_unlocked_serial()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the unlocked serial.
 * \param n The unlocked serial.
 */
void rp::game_variables::set_unlocked_serial( unsigned int n )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( make_persistent_variable_name("scenario/unlocked_serial"), n ) );
} // game_variables::set_unlocked_serial()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the last serial.
 */
unsigned int rp::game_variables::get_last_serial()
{
  return 
    rp_game_variables_get_value
    ( make_persistent_variable_name("scenario/last_serial"), 
      (unsigned int)0 );
} // game_variables::get_last_serial()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the last serial.
 * \param n The last serial.
 */
void rp::game_variables::set_last_serial( unsigned int n )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( make_persistent_variable_name("scenario/last_serial"), n ) );
} // game_variables::set_last_serial()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the selected serial.
 */
unsigned int rp::game_variables::get_selected_serial()
{
  return 
    rp_game_variables_get_value
    ( make_persistent_variable_name("scenario/selected_serial"), 
      (unsigned int)1 );
} // game_variables::get_selected_serial()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the selected serial.
 * \param n The selected serial.
 */
void rp::game_variables::set_selected_serial( unsigned int n )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( make_persistent_variable_name("scenario/selected_serial"), n ) );
} // game_variables::set_selected_serial()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the information of the level.
 */
std::string rp::game_variables::get_level_info()
{
  return rp_game_variables_get_value( "level_info", std::string() );
} // game_variables::get_level_info()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the information of the level.
 * \param n The information of the level.
 */
void rp::game_variables::set_level_info( const std::string& n )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<std::string>( "level_info", n ) );
} // game_variables::set_level_info()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the name of the level.
 */
std::string rp::game_variables::get_level_name()
{
  return rp_game_variables_get_value( "level_name", std::string() );
} // game_variables::get_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the name of the next level to load.
 * \param n The name of the level.
 */
void rp::game_variables::set_level_name( const std::string& n )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<std::string>( "level_name", n ) );
} // game_variables::set_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the name of the level theme.
 */
std::string rp::game_variables::get_level_theme()
{
  return rp_game_variables_get_value( "level_theme", std::string("western") );
} // game_variables::get_level_theme()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the name of the level theme.
 * \param t The name of the level theme.
 */
void rp::game_variables::set_level_theme( const std::string& t )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<std::string>( "level_theme", t ) );
} // game_variables::set_level_theme()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is a demo version.
 */
bool rp::game_variables::is_demo_version()
{
  return rp_game_variables_get_value( "demo_version", false );
} // game_variables::is_demo_version()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if there is a demo version.
 * \param value The new value.
 */
void rp::game_variables::set_demo_version(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "demo_version", value) );
} // game_variables::set_demo_version()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is level ending.
 */
bool rp::game_variables::is_level_ending()
{
  return rp_game_variables_get_value( "level_ending", false );
} // game_variables::is_level_ending()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if there is level ending.
 * \param value The new value.
 */
void rp::game_variables::set_level_ending(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "level_ending", value) );
} // game_variables::set_level_ending()

std::string rp::game_variables::get_ending_effect_variable_name()
{
  return "ending_effect";
}

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the status of ending effect.
 */
bool rp::game_variables::get_ending_effect()
{
  return
    rp_game_variables_get_value( get_ending_effect_variable_name(), false );
} // game_variables::get_ending_effect()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if there is ending_effect.
 * \param value The new value.
 */
void rp::game_variables::set_ending_effect(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( get_ending_effect_variable_name(), value) );
} // game_variables::set_ending_effect()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is a selected level.
 */
bool rp::game_variables::selected_level_exist()
{
  return rp_game_variables_get_value( "selected_level", false );
} // game_variables::selected_level_exist()

/*----------------------------------------------------------------------------*/
/**
 * \brief Select a level.
 * \param value The new value.
 */
void rp::game_variables::select_level(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "selected_level", value) );
} // game_variables::select_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is a go order.
 */
bool rp::game_variables::get_go_order_status()
{
  return rp_game_variables_get_value( "go_order", false );
} // game_variables::get_go_order_status()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set go order status.
 * \param value The new value.
 */
void rp::game_variables::set_go_order_status(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "go_order", value) );
} // game_variables::set_go_order_status()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is a back order.
 */
bool rp::game_variables::get_back_order_status()
{
  return rp_game_variables_get_value( "back_order", false );
} // game_variables::get_back_order_status()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set back order status.
 * \param value The new value.
 */
void rp::game_variables::set_back_order_status(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "back_order", value) );
} // game_variables::set_back_order_status()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is a movement order.
 */
bool rp::game_variables::get_movement_order_status()
{
  return rp_game_variables_get_value( "movement_order", false );
} // game_variables::get_movement_order_status()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set movement order status.
 * \param value The new value.
 */
void rp::game_variables::set_movement_order_status(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "movement_order", value) );
} // game_variables::set_movement_order_status()


/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is a movement order.
 */
bool rp::game_variables::get_in_loading()
{
  return rp_game_variables_get_value( "in_loading", false );
} // game_variables::get_in_loading()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set movement order status.
 * \param value The new value.
 */
void rp::game_variables::set_in_loading(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "in_loading", value) );
} // game_variables::set_in_loading()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the level is a boss level.
 */
bool rp::game_variables::is_boss_level()
{
  return rp_game_variables_get_value( "boss_level", false );
} // game_variables::is_boss_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if te level is a boss level.
 * \param value The new value.
 */
void rp::game_variables::set_boss_level(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "boss_level", value) );
} // game_variables::set_boss_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the level is a boss transition level.
 */
bool rp::game_variables::is_boss_transition()
{
  return rp_game_variables_get_value( "boss_transition", false );
} // game_variables::is_boss_transition()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if te level is a boss transition level.
 * \param value The new value.
 */
void rp::game_variables::set_boss_transition(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "boss_transition", value) );
} // game_variables::set_boss_transition()
    
/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if there is level starting.
 */
bool rp::game_variables::level_has_started()
{
  return rp_game_variables_get_value( "level_starting", false );
} // game_variables::level_has_started()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if there is level starting.
 * \param value The new value.
 */
void rp::game_variables::set_level_starting(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( "level_starting", value) );
} // game_variables::set_level_starting()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for the number of cart elements.
 * \param id The identifier of the cart.
 */
std::string rp::game_variables::get_cart_elements_number_variable_name
(unsigned int id)
{
  std::ostringstream oss;
  oss << "cart_elements_number_" << id;

  return oss.str();
} // game_variables::get_cart_element_number_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of cart elements.
 * \param id The identifier of the cart.
 */
unsigned int rp::game_variables::get_cart_elements_number(unsigned int id)
{
  return rp_game_variables_get_value
    ( get_cart_elements_number_variable_name(id), (unsigned int)0 );
} // game_variables::get_cart_elements_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of cart elements.
 * \param id The identifier of the cart.
 * \param value The new value.
 */
void rp::game_variables::set_cart_elements_number
(unsigned int id, unsigned int value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_cart_elements_number_variable_name(id), value) );
} // game_variables::set_cart_elements_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for the total number of plungers.
 */
std::string rp::game_variables::get_plunger_total_number_variable_name()
{
  return "plunger_total_number";
} // game_variables::get_plunger_total_number_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of plungers.
 */
unsigned int rp::game_variables::get_plunger_total_number()
{
  return rp_game_variables_get_value
    ( get_plunger_total_number_variable_name(), (unsigned int)0 );
} // game_variables::get_plunger_total_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of plungers.
 * \param value The new value.
 */
void rp::game_variables::set_plunger_total_number(unsigned int value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_plunger_total_number_variable_name(), value) );
} // game_variables::set_plunger_total_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for the number of valid plungers.
 */
std::string rp::game_variables::get_plunger_number_variable_name()
{
  return "plunger_number";
} // game_variables::get_plunger_number_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of valid plungers.
 */
unsigned int rp::game_variables::get_plunger_number()
{
  return rp_game_variables_get_value
    ( get_plunger_number_variable_name(), (unsigned int)0 );
} // game_variables::get_plunger_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of valid plungers.
 * \param value The new value.
 */
void rp::game_variables::set_plunger_number(unsigned int value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_plunger_number_variable_name(), value) );
} // game_variables::set_plunger_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for plunger activation.
 */
std::string rp::game_variables::get_plunger_activation_variable_name()
{
  return "plunger_activation";
} // game_variables::get_plunger_activation_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the plunger is valid.
 */
bool rp::game_variables::get_plunger_activation()
{
  return rp_game_variables_get_value
    ( get_plunger_activation_variable_name(), false );
} // game_variables::get_plunger_activation()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if the plunger is valid.
 * \param value The new value.
 */
void rp::game_variables::set_plunger_activation(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>
      ( get_plunger_activation_variable_name(), value) );
} // game_variables::set_plunger_activation()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for plunger validity.
 */
std::string rp::game_variables::get_plunger_validity_variable_name()
{
  return "plunger_validity";
} // game_variables::get_plunger_validity_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the plunger is valid.
 */
bool rp::game_variables::get_plunger_validity()
{
  return rp_game_variables_get_value
    ( get_plunger_validity_variable_name(), false );
} // game_variables::get_plunger_validity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if the plunger is valid.
 * \param value The new value.
 */
void rp::game_variables::set_plunger_validity(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>
      ( get_plunger_validity_variable_name(), value) );
} // game_variables::set_plunger_validity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for cannonball activation.
 */
std::string rp::game_variables::get_cannonball_activation_variable_name()
{
  return "cannonball_activation";
} // game_variables::get_cannonball_activation_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the cannonball is activated.
 */
bool rp::game_variables::get_cannonball_activation()
{
  return rp_game_variables_get_value
    ( get_cannonball_activation_variable_name(), false );
} // game_variables::get_cannonball_activation()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if the cannonball is activated.
 * \param value The new value.
 */
void rp::game_variables::set_cannonball_activation(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>
      ( get_cannonball_activation_variable_name(), value) );
} // game_variables::set_cannonball_activation()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for cannonball validity.
 */
std::string rp::game_variables::get_cannonball_validity_variable_name()
{
  return "cannonball_validity";
} // game_variables::get_cannonball_validity_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the cannonball is valid.
 */
bool rp::game_variables::get_cannonball_validity()
{
  return rp_game_variables_get_value
    ( get_cannonball_validity_variable_name(), false );
} // game_variables::get_cannonball_validity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if the cannonball is valid.
 * \param value The new value.
 */
void rp::game_variables::set_cannonball_validity(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>
      ( get_cannonball_validity_variable_name(), value) );
} // game_variables::set_cannonball_validity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for status visibility.
 */
std::string rp::game_variables::get_status_visibility_variable_name()
{
  return "status_visibility";
} // game_variables::get_status_visibility_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the cannonball is activated.
 */
bool rp::game_variables::get_status_visibility()
{
  return rp_game_variables_get_value
    ( get_status_visibility_variable_name(), false );
} // game_variables::get_status_visibility()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if the cannonball is activated.
 * \param value The new value.
 */
void rp::game_variables::set_status_visibility(bool value)
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>
      ( get_status_visibility_variable_name(), value) );
} // game_variables::set_status_visibility()


/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for score of a given level.
 * \param serial The serial of the level.
 * \param number The number of the level.
 */
std::string rp::game_variables::get_persistent_score_variable_name
(unsigned int serial, unsigned int number)
{ 
  std::ostringstream stream;
  stream << "level/" << serial << "/" << number << "/score";

  return make_persistent_variable_name(stream.str());
} // game_variables::get_persistent_score_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for score.
 */
std::string rp::game_variables::get_score_variable_name()
{
  return "score";
} // game_variables::get_score_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the persistent score of a given level.
 */
unsigned int rp::game_variables::get_persistent_score()
{
  return rp_game_variables_get_value
    ( get_persistent_score_variable_name
      ( game_variables::get_serial_number(),
        game_variables::get_level_number() ),
      (unsigned int)0 );
} // game_variables::get_persistent_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the persistent score of a given level.
 * \param serial The serial of the level.
 * \param number The number of the level.
 */
unsigned int rp::game_variables::get_persistent_score
(unsigned int serial, unsigned int number)
{
  return rp_game_variables_get_value
    ( get_persistent_score_variable_name( serial,number ), (unsigned int)0 );
} // game_variables::get_persistent_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the persistent score of a given level.
* \param c The number of score.
 */
void rp::game_variables::set_persistent_score( unsigned int c )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_persistent_score_variable_name
         ( game_variables::get_serial_number(),
           game_variables::get_level_number() ) , c ) );
} // game_variables::set_persistent_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the score.
 */
unsigned int rp::game_variables::get_score()
{
  return rp_game_variables_get_value
    ( get_score_variable_name() , (unsigned int)0 );
} // game_variables::get_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the score.
 * \param c The number of score.
 */
void rp::game_variables::set_score( unsigned int c )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_score_variable_name() , c ) );
} // game_variables::set_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add or remove some points to the score.
 * \param combo The value of the combo.
 * \param points The number of points.
 * \param floating_score Indicates if we create a floating score.
 */
void rp::game_variables::add_score
( unsigned int combo, int points, bool floating_score )
{
  set_combo(combo);
  set_points(points);

  if ( floating_score )
    set_new_score();
  else
    {
      int c = combo * points;
      if ( (c < 0) && ((int)get_score() < -c) )
        set_score( 0 );
      else
        set_score( get_score() + c );
    }
} // game_variables::add_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for new score state.
 */
std::string rp::game_variables::get_new_score_variable_name()
{
  return "new_score";
} // game_variables::get_new_score_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set new score state.
 * \param value The new value.
 */
void rp::game_variables::set_new_score()
{
  static const std::string name( get_new_score_variable_name() );
  const bool value( rp_game_variables_get_value( name, false ) );
  
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( name, !value ) );
} // game_variables::set_new_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for new score state.
 */
std::string rp::game_variables::get_last_combo_variable_name()
{
  return "last_combo";
} // game_variables::get_last_combo_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the last combo.
 */
unsigned int rp::game_variables::get_last_combo()
{
  return rp_game_variables_get_value
    ( get_last_combo_variable_name(), (unsigned int)0 );
} // game_variables::get_combo()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the last combo.
 * \param c The last combo value.
 */
void rp::game_variables::set_last_combo( unsigned int last_combo )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_last_combo_variable_name(), last_combo ) );
} // game_variables::set_last_combo()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for level state.
 * \param serial The serial of the level.
 * \param number The number of the level.
 */
std::string rp::game_variables::get_level_state_variable_name
(unsigned int serial, unsigned int number)
{ 
  std::ostringstream stream;
  stream << "level/" << serial << "/" << number << "/level_state";

  return make_persistent_variable_name(stream.str());
} // game_variables::get_level_state_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the level state of a given level.
 * \param serial The serial of the level.
 * \param number The number of the level.
 */
unsigned int rp::game_variables::get_level_state
(unsigned int serial, unsigned int number)
{
  return rp_game_variables_get_value
    ( get_level_state_variable_name( serial,number ), (unsigned int)0 );
} // game_variables::get_level_state()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the level state of a given level.
 * \param serial The serial of the level.
 * \param number The number of the level.
 * \param c The level_state.
 */
void rp::game_variables::set_level_state
( unsigned int serial, unsigned int number, unsigned int c )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_level_state_variable_name( serial, number ) , c ) );
} // game_variables::set_level_state()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the bronze threshold.
 */
unsigned int rp::game_variables::get_last_medal()
{
  return rp_game_variables_get_value( "medal/last", (unsigned int)0 );
} // game_variables::get_last_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the bronze threshold.
 * \param threshold The threshold for bronze medal.
 */
void rp::game_variables::set_last_medal( unsigned int threshold )
{  
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>( "medal/last" , threshold ) );
} // game_variables::set_last_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the bronze threshold.
 */
unsigned int rp::game_variables::get_bronze_threshold()
{
  return rp_game_variables_get_value
    ( "medal/bronze/threshold", (unsigned int)0 );
} // game_variables::get_bronze_threshold()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the bronze threshold.
 * \param threshold The threshold for bronze medal.
 */
void rp::game_variables::set_bronze_threshold( unsigned int threshold )
{  
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( "medal/bronze/threshold" , threshold ) );
} // game_variables::set_bronze_threshold()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the silver threshold.
 */
unsigned int rp::game_variables::get_silver_threshold()
{
  return rp_game_variables_get_value
    ( "medal/silver/threshold", (unsigned int)0 );
} // game_variables::get_silver_threshold()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the silver threshold.
 * \param threshold The threshold for silver medal.
 */
void rp::game_variables::set_silver_threshold( unsigned int threshold )
{  
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( "medal/silver/threshold" , threshold ) );
} // game_variables::set_silver_threshold()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the gold threshold.
 */
unsigned int rp::game_variables::get_gold_threshold()
{
  return rp_game_variables_get_value
    ( "medal/gold/threshold", (unsigned int)0 );
} // game_variables::get_gold_threshold()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the gold threshold.
 * \param threshold The threshold for gold medal.
 */
void rp::game_variables::set_gold_threshold( unsigned int threshold )
{  
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( "medal/gold/threshold" , threshold ) );
} // game_variables::set_gold_threshold()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for combo.
 */
std::string rp::game_variables::get_combo_variable_name()
{
  return "combo";
} // game_variables::get_combo_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the combo value.
 */
unsigned int rp::game_variables::get_combo()
{
  return rp_game_variables_get_value
    ( get_combo_variable_name(), (unsigned int)0 );
} // game_variables::get_combo()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the combo value.
 * \param nb The value of the combo.
 */
void rp::game_variables::set_combo( unsigned int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      (  get_combo_variable_name(), nb ) );
} // game_variables::set_combo()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for points.
 */
std::string rp::game_variables::get_points_variable_name()
{
  return "points";
} // game_variables::get_points_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the points.
 */
int rp::game_variables::get_points()
{
  return rp_game_variables_get_value
    ( get_points_variable_name(), (int)0 );
} // game_variables::get_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set points value.
 * \param nb The value of points.
 */
void rp::game_variables::set_points( int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<int>
      (  get_points_variable_name(), nb ) );
} // game_variables::set_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the x-position rate of corrupting bonus according camera center.
 */
double rp::game_variables::get_score_rate_x()
{
  return rp_game_variables_get_value( "score_rate_x", (double)0 );
} // game_variables::get_score_rate_x()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the x-position rate of corrupting bonus according camera center.
 * \param x The rate on x-coordinate.
 */
void rp::game_variables::set_score_rate_x(double x)
{
   bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<double>
      ( "score_rate_x", x ) );
} // game_variables::set_score_rate_x()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the y-position rate of corrupting bonus according camera center.
 */
double rp::game_variables::get_score_rate_y()
{
  return rp_game_variables_get_value( "score_rate_y", (double)0 );
} // game_variables::get_score_rate_y()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the y-position rate of corrupting bonus according camera center.
 * \param y The rate on y_coordinate.
 */
void rp::game_variables::set_score_rate_y(double y)
{
   bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<double>
      ( "score_rate_y", y ) );
} // game_variables::set_score_rate_y()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for boss hits.
 */
std::string rp::game_variables::get_boss_hits_variable_name()
{
  return "boss_hits";
} // game_variables::get_boss_hits_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of boss hits.
 */
unsigned int rp::game_variables::get_boss_hits()
{
  return rp_game_variables_get_value
    ( get_boss_hits_variable_name(), (unsigned int)0 );
} // game_variables::get_boss_hits()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of hits on boss.
 * \param nb The number of found boss_hits.
 */
void rp::game_variables::set_boss_hits( unsigned int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      (  get_boss_hits_variable_name(), nb ) );
} // game_variables::set_boss_hits()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for balloon of a given level.
 * \param serial The serial of the level.
 * \param number The number of the level.
 */
std::string rp::game_variables::get_persistent_balloon_variable_name
(unsigned int serial, unsigned int number)
{ 
  std::ostringstream stream;
  stream << "level/" << serial << "/" << number << "/balloon";

  return make_persistent_variable_name(stream.str());
} // game_variables::get_persistent_balloon_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the persistent balloon of a given level.
 */
unsigned int rp::game_variables::get_persistent_balloon()
{
  return rp_game_variables_get_value
    ( get_persistent_balloon_variable_name
      ( game_variables::get_serial_number(),
        game_variables::get_level_number() ),
      (unsigned int)0 );
} // game_variables::get_persistent_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the persistent balloon of a given level.
 * \param serial The serial of the level.
 * \param number The number of the level.
 */
unsigned int rp::game_variables::get_persistent_balloon
(unsigned int serial, unsigned int number)
{
  return rp_game_variables_get_value
    ( get_persistent_balloon_variable_name( serial, number ), (unsigned int)0 );
} // game_variables::get_persistent_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the persistent balloon of a given level.
 * \param b The number of balloon.
 */
void rp::game_variables::set_persistent_balloon( unsigned int b )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      ( get_persistent_balloon_variable_name
         ( game_variables::get_serial_number(),
           game_variables::get_level_number() ) , b ) );
} // game_variables::set_persistent_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for balloon number.
 */
std::string rp::game_variables::get_balloon_variable_name()
{
  return "balloon_number";
} // game_variables::get_balloon_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of balloons.
 */
unsigned int rp::game_variables::get_balloons_number()
{
  return rp_game_variables_get_value
    ( get_balloon_variable_name(), (unsigned int)0 );
} // game_variables::get_balloons_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of balloons.
 * \param nb The number of found balloons.
 */
void rp::game_variables::set_balloons_number( unsigned int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      (  get_balloon_variable_name(), nb ) );
} // game_variables::set_balloons_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of required balloons.
 */
unsigned int
rp::game_variables::get_required_balloons_number()
{
  return rp_game_variables_get_value
    ( "required_balloons_number", (unsigned int)0 );
} // game_variables::get_required_balloons_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of required balloons.
 * \param nb The number of found balloons.
 */
void rp::game_variables::set_required_balloons_number( unsigned int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>( "required_balloons_number", nb ) );
} // game_variables::set_required_balloons_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for bad_balloon number.
 */
std::string rp::game_variables::get_bad_balloon_variable_name()
{
  return "bad_balloon_number";
} // game_variables::get_bad_balloon_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of bad_balloon.
 */
unsigned int rp::game_variables::get_bad_balloon_number()
{
  return rp_game_variables_get_value
    ( get_bad_balloon_variable_name(), (unsigned int)0 );
} // game_variables::get_bad_balloon_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of bad_balloon.
 * \param nb The number of found bad_balloon.
 */
void rp::game_variables::set_bad_balloon_number( unsigned int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      (  get_bad_balloon_variable_name(), nb ) );
} // game_variables::set_bad_balloon_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for bad_plunger number.
 */
std::string rp::game_variables::get_bad_plunger_variable_name()
{
  return "bad_plunger_number";
} // game_variables::get_bad_plunger_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of bad_plunger.
 */
unsigned int rp::game_variables::get_bad_plunger_number()
{
  return rp_game_variables_get_value
    ( get_bad_plunger_variable_name(), (unsigned int)0 );
} // game_variables::get_bad_plunger_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of bad_plunger.
 * \param nb The number of found bad_plunger.
 */
void rp::game_variables::set_bad_plunger_number( unsigned int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      (  get_bad_plunger_variable_name(), nb ) );
} // game_variables::set_bad_plunger_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the variable name for bad_cannonball number.
 */
std::string rp::game_variables::get_bad_cannonball_variable_name()
{
  return "bad_cannonball_number";
} // game_variables::get_bad_cannonball_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the number of bad_cannonball.
 */
unsigned int rp::game_variables::get_bad_cannonball_number()
{
  return rp_game_variables_get_value
    ( get_bad_cannonball_variable_name(), (unsigned int)0 );
} // game_variables::get_bad_cannonball_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the number of bad_cannonball.
 * \param nb The number of found bad_cannonball.
 */
void rp::game_variables::set_bad_cannonball_number( unsigned int nb )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<unsigned int>
      (  get_bad_cannonball_variable_name(), nb ) );
} // game_variables::set_bad_cannonball_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get red intenisty of last balloon.
 */
double rp::game_variables::get_balloon_red_intensity()
{
  return rp_game_variables_get_value
    ( "balloon_red_intensity", (double)1 );
} // game_variables::get_balloon_red_intensity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set red intensity of last balloon.
 * \param red The red intensity of the last balloon.
 */
void rp::game_variables::set_balloon_red_intensity( double red )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<double>
      ( "balloon_red_intensity", red ) );
} // game_variables::set_balloon_red_intensity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get green intenisty of last balloon.
 */
double rp::game_variables::get_balloon_green_intensity()
{
  return rp_game_variables_get_value
    ( "balloon_green_intensity", (double)1 );
} // game_variables::get_balloon_green_intensity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set green intensity of last balloon.
 * \param green The green intensity of the last balloon.
 */
void rp::game_variables::set_balloon_green_intensity( double green )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<double>
      ( "balloon_green_intensity", green ) );
} // game_variables::set_balloon_green_intensity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get blue intenisty of last balloon.
 */
double rp::game_variables::get_balloon_blue_intensity()
{
  return rp_game_variables_get_value
    ( "balloon_blue_intensity", (double)1 );
} // game_variables::get_balloon_blue_intensity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set blue intensity of last balloon.
 * \param blue The blue intensity of the last balloon.
 */
void rp::game_variables::set_balloon_blue_intensity( double blue )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<double>
      ( "balloon_blue_intensity", blue ) );
} // game_variables::set_balloon_blue_intensity()

void rp::game_variables::schedule_interstitial( bool b )
{
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>
      ( "interstitial_scheduled", b ) );
}

bool rp::game_variables::interstitial_scheduled()
{
  return rp_game_variables_get_value( "interstitial_scheduled", false );
}

/*----------------------------------------------------------------------------*/
/**
 * \brief Get a variable name prefixed with persistent option prefix.
 * \param n The name of the variable.
 */
std::string rp::game_variables::make_persistent_variable_name
( const std::string& n )
{
  return RP_PERSISTENT_PREFIX + n;
} // game_variables::make_persistent_variable_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load persistent variables.
 * \param lvl The filename of the level.
 */
void rp::game_variables::load_variables(const std::string& lvl)
{
} // game_variables::load_variables()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save persistent variables. 
 * \param lvl The filename of the level.
 */
void rp::game_variables::save_variables(const std::string& lvl)
{
} // game_variables::save_variables()

std::string rp::game_variables::get_action_snapshot_variable_name()
{
  return "action_snapshot";
}

void rp::game_variables::set_action_snapshot()
{
  static const std::string name( get_action_snapshot_variable_name() );
  const bool value( rp_game_variables_get_value( name, false ) );
  
  bear::engine::game::get_instance().set_game_variable
    ( bear::engine::variable<bool>( name, !value ) );
}

