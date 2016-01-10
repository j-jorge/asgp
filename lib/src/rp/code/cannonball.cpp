/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the cannonball class.
 * \author Sebastien Angibaud
 */
#include "rp/cannonball.hpp"
#include "rp/cable.hpp" 
#include "rp/cart.hpp"
#include "rp/game_variables.hpp"
#include "rp/tar.hpp"
#include "rp/util.hpp"

#include "engine/level.hpp"
#include "universe/collision_info.hpp"
#include "universe/zone.hpp"
#include "universe/forced_movement/forced_translation.hpp"

#include "generic_items/path_trace.hpp"

BASE_ITEM_EXPORT( cannonball, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::cannonball::cannonball()
: m_cart(NULL), m_combo_value(1)
{
  set_phantom(true);
  set_mass(1);
  set_system_angle_as_visual_angle( true );
} // rp::cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::cannonball::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_model("model/cannonball.cm");
  get_level_globals().load_sound("sound/hit-2.ogg");
} // rp::cannonball::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::cannonball::on_enters_layer()
{
  super::on_enters_layer();
  
  set_model_actor( get_level_globals().get_model("model/cannonball.cm") );
  start_model_action("idle");
  m_sight_position = NULL;

  create_trace();
} // rp::cannonball::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::cannonball::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );
  
  if ( ( get_center_of_mass().distance
         (m_sight_position->get_center_of_mass()) <= 30 ) ||
       ( get_speed().x > 0 && 
         get_horizontal_middle() > 
         m_sight_position->get_horizontal_middle() ) ||
       ( get_speed().x < 0 && 
         get_horizontal_middle() < 
         m_sight_position->get_horizontal_middle() ) )
    clear_forced_movement();

  const bear::universe::vector_type speed( get_speed() );
  if ( ( speed.x != 0 ) && ( speed.y != 0 ) )
    set_system_angle( std::atan2( speed.y, speed.x ) );
  
  bear::universe::rectangle_type r = get_level().get_camera_focus();
  bear::universe::rectangle_type rect
    (r.left() - 50, r.bottom() - 50, r.right() + 50, r.top() + 50 );
  if ( ! rect.intersects( get_bounding_box() ) )
    {
      game_variables::set_bad_cannonball_number
        ( game_variables::get_bad_cannonball_number() + 1);
      kill();
    }
} // cannonball::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Kill the cannonball.
 */
void rp::cannonball::kill()
{
  if ( m_sight_position != NULL )
    {
      m_sight_position->kill();
      m_sight_position = NULL;
    }

  super::kill();
} // cannonball::kill()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create movement of the object.
 * \param dir Direction of the fire.
 * \param cart_speed Speed of the cart.
 * \param sight_pos The position of the sight.
 * \param good_fire_angle Indicates if fire angle is valid.
 */
void rp::cannonball::create_movement
( const bear::universe::vector_type& dir, 
  const bear::universe::speed_type& cart_speed,
  const bear::universe::position_type& sight_pos, bool good_fire_angle)
{
  bear::universe::forced_translation mvt;
  
  bear::universe::vector_type speed(dir);  
  speed.normalize();
  speed *= (2000 + cart_speed.length());
  mvt.set_speed(speed);
  
  m_sight_position = new bear::reference_item;
  bear::universe::position_type pos(sight_pos);
  
  if ( ( sight_pos.distance(get_center_of_mass()) < 100 ) )
    {
      pos.x = get_horizontal_middle() + dir.x * 100;
      pos.y = get_vertical_middle() + dir.y * 100;
    }
  
  m_sight_position->set_center_of_mass(pos);
  new_item(*m_sight_position);

  mvt.set_auto_remove(true);
  set_forced_movement( mvt );
} // cannonball::create_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::cannonball::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  if ( ! collision_with_cable(that,info) )
    collision_with_tar(that);
} // cannonball::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set cart.
 * \param c The cart.
 */
void rp::cannonball::set_cart( cart* c )
{
  m_cart = c;
} // cannonball::set_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get cart.
 */
rp::cart* rp::cannonball::get_cart()
{
  return m_cart;
} // cannonball::get_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the value of the combo.
 */
unsigned int rp::cannonball::get_combo_value() const
{
  return m_combo_value;
} // rp::cannonball::get_combo_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cable.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cable.
 */
bool rp::cannonball::collision_with_cable
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  cable* c = dynamic_cast<cable*>(&that);
  
  if ( c != NULL ) 
    {
      c->clear_forced_movement();
      bear::universe::speed_type speed = get_speed();
      bear::universe::force_type f(speed);
      f.normalize();
      c->add_external_force(c->get_mass()*f*20000);

      if ( ( info.get_collision_side() == bear::universe::zone::top_zone ) ||
           ( info.get_collision_side() == bear::universe::zone::bottom_zone ) )
        speed.y *= -1;
      else 
        speed.x *= -1;
      set_speed(speed);

      result = true;

      get_level_globals().play_sound
        ( "sound/hit-2.ogg",
          bear::audio::sound_effect( get_center_of_mass() ) );
    }
  
  return result;
} // cannonball::collision_with_cable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tar.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a tar.
 */
bool rp::cannonball::collision_with_tar( bear::engine::base_item& that )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {
      if ( t->get_current_action_name() == "idle" ) 
        {
          t->set_combo_value(1);
          util::create_floating_score(*t,100);
          t->kill();
          clear_forced_movement();
          set_speed(get_speed()/3);
          m_combo_value = 2;
        }

      result = true;
    }
  
  return result;
} // cannonball::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates a trace that follows the cannonball.
 */
void rp::cannonball::create_trace() const
{
  bear::path_trace* trace = new bear::path_trace( *this );
  
  trace->set_tail_ratio( 0 );
  trace->set_length( 0.1 );
  trace->set_global( true );
  trace->set_fade_out_speed( std::numeric_limits<double>::infinity() );
  trace->set_fill_color( bear::visual::color_type("#40000000") );

  new_item( *trace );
} // cannonball::create_trace()
