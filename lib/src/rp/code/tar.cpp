/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the tar class.
 * \author Sebastien Angibaud
 */
#include "rp/tar.hpp"

#include "rp/cart.hpp"
#include "rp/game_variables.hpp"
#include "rp/plank.hpp"

#include <boost/algorithm/string/predicate.hpp>

BASE_ITEM_EXPORT( tar, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::tar::tar()
: m_cart_contact(false)
{
  set_system_angle_as_visual_angle(true);
  set_mass(0.1);
  set_density(0.002);
} // rp::tar::tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::tar::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/tar.cm");
  get_level_globals().load_sound("sound/tar/splash.ogg");
} // rp::tar::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::tar::on_enters_layer()
{
  super::on_enters_layer();

  set_model_actor( get_level_globals().get_model("model/tar.cm") );
  start_model_action("idle");
  create_interactive_item(*this);
} // rp::tar::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do one step in the progression of the item.
 * \param elapsed_time Elasped time since the last progress.
 */
void rp::tar::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );
  const std::string action_name = get_current_action_name();

  if ( ( action_name == "idle" ) && !has_forced_movement() )
    fall();
  else if ( ( action_name != "explose" )
            && !boost::starts_with(action_name, "on_rail")
            && has_bottom_contact() )
    {
      kill_interactive_item();
      clear_forced_movement();
      
      start_model_action("on_rail");

      set_z_position( get_z_position() + 110 );
      set_z_fixed( true );

      if ( get_system_angle() != 0 )
        add_position_constraint_x();

      get_level_globals().play_sound
        ( "sound/tar/splash.ogg",
          bear::audio::sound_effect( get_center_of_mass() ) );
    }
  else if ( (action_name == "on_rail_spatter") && !m_cart_contact )
    start_model_action( "on_rail_idle" );

  m_cart_contact = false;
} // rp::tar::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::tar::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  if ( ! collision_with_plank(that) )
    collision_with_cart(that);
} // tar::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start fall state.
 */
void rp::tar::fall()
{
  clear_forced_movement();
  start_model_action("fall");
} // tar::fall()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start explose state.
 */
void rp::tar::explose()
{
  if ( get_current_action_name() != "explose" )
    start_model_action("explose");
} // tar::explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::tar::populate_loader_map( bear::engine::item_loader_map& m )
{
  super::populate_loader_map( m );

  m.insert( entity::loader( *this ) );
} // tar::populate_loader_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a plank.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a plank.
 */
bool rp::tar::collision_with_plank
( bear::engine::base_item& that )
{ 
  bool result = false;
  plank* p = dynamic_cast<plank*>(&that);
  
  if ( p != NULL ) 
    {
      if ( get_current_action_name() == "idle"  )
        {
          game_variables::set_action_snapshot();
          if ( p->get_combo_value() != 0 )
            set_combo_value(p->get_combo_value()+1);          
          fall();
        }
      
      result = true;
    }
  
  return result;
} // tar::collision_with_plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::tar::collision_with_cart( bear::engine::base_item& that )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      game_variables::set_action_snapshot();

      const std::string action_name( get_current_action_name() );

      if ( boost::starts_with( get_current_action_name(), "on_rail") ) 
        c->set_can_jump(false);

      if ( (action_name != "on_rail_spatter") && ( action_name != "explose" ) )
        start_model_action( "on_rail_spatter" );

      m_cart_contact = true;
      result = true;
    }
  
  return result;
} // tar::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the explosition is finished.
 */
void rp::tar::finish_explose()
{ 
  kill();
} // tar::finish_explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::tar::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::tar, finish_explose, void );
} // tar::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::tar )
