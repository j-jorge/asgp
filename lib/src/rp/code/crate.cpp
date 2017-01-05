/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the crate class.
 * \author Sebastien Angibaud
 */
#include "rp/crate.hpp"
#include "rp/cable.hpp"
#include "rp/cannonball.hpp"
#include "rp/cart.hpp"
#include "rp/explosion.hpp"
#include "rp/game_variables.hpp"
#include "rp/util.hpp"

BASE_ITEM_EXPORT( crate, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::crate::crate()
{
  set_mass(100);
  set_system_angle_as_visual_angle(true);
} // rp::crate::rp::crate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::crate::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/crate.cm");
} // rp::crate::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::crate::on_enters_layer()
{
  super::on_enters_layer();

  set_model_actor( get_level_globals().get_model("model/crate.cm") );
  start_model_action("idle");
} // rp::crate::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::crate::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  if ( ! collision_with_cart(that) )
    if ( ! collision_with_cannonball(that) )
      if ( ! collision_with_explosion(that, info) )
        if ( ! collision_with_cable(that) )
          super::collision(that, info);
} // crate::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::crate::collision_with_cart( bear::engine::base_item& that )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      if ( get_current_action_name() == "idle" ) 
        {
          set_combo_value(0);          
          c->is_hit();
          explose(false);
        }

      result = true;
    }
  
  return result;
} // crate::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::crate::collision_with_cannonball
( bear::engine::base_item& that )
{ 
  bool result = false;
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL ) 
    {
      if ( get_current_action_name() == "idle" )
        {
          set_combo_value( c->get_combo_value() );          
          explose();
          c->kill();
        }
      result = true;
    }
  
  return result;
} // crate::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cable.
 */
bool rp::crate::collision_with_cable
( bear::engine::base_item& that )
{ 
  bool result = false;
  cable* c = dynamic_cast<cable*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ( get_current_action_name() == "idle" ) && 
           ( c->is_ejected() ) )
        {
          if ( c->get_combo_value() != 0 )
            set_combo_value(c->get_combo_value()+1);          
          explose();
        }
      result = true;
    }
  
  return result;
} // crate::collision_with_cable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::crate::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( e != NULL ) 
    {
      if ( get_current_action_name() == "idle" && e->test_in_explosion(info) )
        {
          if ( e->get_combo_value() != 0 )
            set_combo_value(e->get_combo_value()+1);          
          explose();
        }

      result = true;
    }
  
  return result;
} // crate::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the crate explose.
 * \param give_score Indicates if we give score.
 */
void rp::crate::explose(bool give_score)
{ 
  game_variables::set_action_snapshot();
  set_transportability(false);
  kill_interactive_item();

  if ( give_score )
    util::create_floating_score(*this,500);
  
  set_can_move_items(false);
  start_model_action("explose");

  create_planks("plank");
} // crate::explose()

