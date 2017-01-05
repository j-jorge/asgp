/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the cable class.
 * \author Sebastien Angibaud
 */
#include "rp/cable.hpp"
#include "rp/cart.hpp"
#include "rp/explosion.hpp"
#include "rp/game_variables.hpp"
#include "rp/util.hpp"

#include "universe/collision_info.hpp"
#include "universe/zone.hpp"

BASE_ITEM_EXPORT( cable, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::cable::cable()
: m_is_ejected(false), m_has_hit(false)
{
  set_system_angle_as_visual_angle(true);
} // rp::cable::rp::cable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::cable::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/cable.cm");
  get_level_globals().load_sound("sound/boing.ogg");
} // rp::cable::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::cable::on_enters_layer()
{
  super::on_enters_layer();

  set_mass(100);
  set_model_actor( get_level_globals().get_model("model/cable.cm") );
  start_model_action("idle");
} // rp::cable::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::cable::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  if ( m_is_ejected )
    set_weak_collisions(true);
} // rp::cable::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::cable::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  if ( ! collision_with_cart(that, info) )
    if ( ! collision_with_explosion(that, info) )
      if ( ! collision_with_cable(that, info) )
        super::collision(that, info);
} // cable::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if cable is ejected.
 */
bool rp::cable::is_ejected() const
{
  return m_is_ejected;
} // cable::is_ejected()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::cable::collision_with_cart
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ( info.get_collision_side() == bear::universe::zone::top_zone ) &&
           c->get_current_action_name() != "jump" && 
            ! m_is_ejected )
        c->jump();
      else if (info.get_collision_side() != bear::universe::zone::middle_zone)
        {
          if ( ! m_is_ejected )
            {
              set_combo_value(0);          
              eject
                ( c->get_horizontal_middle() < get_horizontal_middle(), false );
            }
          
          if ( ! m_has_hit )
            {
              m_has_hit = true;
              c->is_hit();
            }
        }

      game_variables::set_action_snapshot();
      result = true;
    }
  
  return result;
} // cable::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cable.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cable.
 */
bool rp::cable::collision_with_cable
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  cable* c = dynamic_cast<cable*>(&that);
  
  if ( c != NULL ) 
    {
      if ( c->is_ejected() && ! m_is_ejected )
        {
          if ( c->get_combo_value() != 0 )
            set_combo_value(c->get_combo_value()+1);            
          eject( c->get_horizontal_middle() < get_horizontal_middle() );
        }
      super::collision(that, info);
      result = true;
      game_variables::set_action_snapshot();
    }
  
  return result;
} // cable::collision_with_cable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::cable::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( e != NULL ) 
    {
      if ( ! m_is_ejected && e->test_in_explosion(info) )
        {
          if ( e->get_combo_value() != 0 )
            set_combo_value(e->get_combo_value()+1);          
          eject( e->get_horizontal_middle() < get_horizontal_middle() );
        }
  
      game_variables::set_action_snapshot();
      result = true;
    }
  
  return result;
} // cable::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Eject the cable.
 * \param on_right Inform that the cable must be ejected on the right.
 * \param give_score Indicates if we give score.
 */
void rp::cable::eject(bool on_right, bool give_score)
{ 
  kill_interactive_item();

  if ( give_score )
    util::create_floating_score(*this,1000);

  set_phantom(true);
  m_is_ejected = true;

  if ( on_right )
    {
      set_angular_speed( -4 );
      add_external_force(bear::universe::force_type(4000000,10000000));
    }
  else
    {
      add_external_force(bear::universe::force_type(-4000000,10000000));
      set_angular_speed( 4 );
    }

  set_system_angle_as_visual_angle(true);
  
  get_level_globals().play_sound
    ( "sound/boing.ogg", bear::audio::sound_effect( get_center_of_mass() ) );
} // cable::eject()
