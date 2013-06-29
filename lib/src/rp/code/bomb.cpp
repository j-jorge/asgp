/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bomb class.
 * \author Sebastien Angibaud
 */
#include "rp/bomb.hpp"
#include "rp/bird.hpp"
#include "rp/cannonball.hpp"
#include "rp/plank.hpp"
#include "rp/cart.hpp"
#include "rp/explosion.hpp"
#include "rp/obstacle.hpp"
#include "rp/tar.hpp"
#include "rp/util.hpp"
#include "rp/wall.hpp"
#include "rp/zeppelin.hpp"

#include "universe/forced_movement/forced_stay_around.hpp"
#include "universe/forced_movement/forced_tracking.hpp"

#include "generic_items/decorative_item.hpp"
#include "generic_items/decorative_effect.hpp"

#include <boost/math/constants/constants.hpp>

BASE_ITEM_EXPORT( bomb, rp )

const double rp::bomb::s_initial_mass = 1;

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::bomb::bomb()
: m_explosed(false)
{
  set_mass(s_initial_mass);
  set_density(0.002);
  set_elasticity(1.5);
  
  set_system_angle
    ( -boost::math::constants::pi<double>() / 3
      + (double)rand() / RAND_MAX * boost::math::constants::pi<double>() / 6 );

  set_system_angle_as_visual_angle( true );
} // rp::bomb::bomb()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::bomb::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/bomb.cm");
  get_level_globals().load_animation("animation/explosion.canim");
} // rp::bomb::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::bomb::on_enters_layer()
{
  super::on_enters_layer();

  set_model_actor( get_level_globals().get_model("model/bomb.cm") );
  start_model_action("idle");
  create_interactive_item(*this);
} // rp::bomb::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::bomb::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  if ( has_bottom_contact() )
    { 
      if ( ! has_forced_movement() )
        {
          set_density(1);
          set_elasticity(1);
        }
      else
        clear_forced_movement();
    }
} // bomb::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the type of the attracted item.
 */
std::string rp::bomb::get_type() const
{
  return "bomb";
} // bomb::get_type()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when cart takes the object.
 * \param c The cart that take the object
 */
void rp::bomb::be_taken(cart* c)
{
  super::be_taken(c);

  explose();
} // bomb::be_taken()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::bomb::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  if ( ! collision_with_cannonball(that) )
    if ( ! collision_with_explosion(that,info) )
      if ( ! collision_with_cart(that) )
        if ( ! collision_with_zeppelin(that) )
          if ( ! collision_with_wall(that) )
            if ( ! collision_with_plank(that) )
              if ( ! collision_with_tar(that,info) )
                if ( ! collision_with_bird(that) )
                  collision_with_obstacle(that);
} // bomb::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the bomb exploses.
 */
void rp::bomb::explose()
{ 
  set_transportability(false);
  kill_interactive_item();
  m_explosed = true;
  start_model_action("explose");
  set_phantom(true);
  set_mass(std::numeric_limits<double>::infinity());
  util::create_floating_score(*this,100);

  explosion* item = new explosion(1, 0, 0.4);
  item->set_z_position(get_z_position() + 1000);
  item->set_combo_value(get_combo_value());
  item->set_center_of_mass(get_center_of_mass());
  new_item( *item );
  CLAW_ASSERT( item->is_valid(),
               "The explosion of bomb isn't correctly initialized" );

  if ( get_attracted_state() )
    {
      leave();
      clear_forced_movement();
    }
} // bomb::explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return if the bomb is explosed.
 */
bool rp::bomb::is_explosed() const
{ 
  return m_explosed;
} // bomb::is_explosed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Indicates if the item can be attractable.
 */
bool rp::bomb::is_attractable() const
{
  return ( get_current_action_name() == "idle" ) &&
    super::is_attractable();
} // bomb::is_attractable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::bomb::collision_with_cannonball( bear::engine::base_item& that )
{ 
  bool result = false;
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ! m_explosed )
        {
          set_combo_value( c->get_combo_value() );
          explose();
          c->kill();
        }

      result = true;
    }
  
  return result;
} // bomb::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with an explosion.
 */
bool rp::bomb::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( e != NULL ) 
    {
      if ( ! m_explosed && e->test_in_explosion(info) )
        {
          if ( e->get_combo_value() != 0 )
            set_combo_value(e->get_combo_value()+1);
          explose();
        }

      result = true;
    }
  
  return result;
} // bomb::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::bomb::collision_with_obstacle( bear::engine::base_item& that )
{ 
  bool result = false;
  obstacle* o = dynamic_cast<obstacle*>(&that);
  
  if ( o != NULL ) 
    {
      if ( ! m_explosed )
        explose();

      result = true;
    }
  
  return result;
} // bomb::collision_with_obstacle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::bomb::collision_with_cart( bear::engine::base_item& that )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ! m_explosed && ! get_attracted_state() ) 
        {
          set_combo_value(0);
          explose();
          c->is_hit();
        }
      
      result = true;
    }
  
  return result;
} // bomb::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a wall.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a wall.
 */
bool rp::bomb::collision_with_wall( bear::engine::base_item& that )
{ 
  bool result = false;
  wall* w = dynamic_cast<wall*>(&that);
  
  if ( w != NULL ) 
    {
      if ( ! m_explosed )
        {
          explose();
          w->set_combo_value(get_combo_value() + 1);
          w->hit(get_bottom(),get_top());
        }

      result = true;
    }
  
  return result;
} // bomb::collision_with_wall()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with hot air balloon.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a hot air balloon.
 */
bool rp::bomb::collision_with_zeppelin( bear::engine::base_item& that )
{ 
  bool result = false;
  zeppelin* z = dynamic_cast<zeppelin*>(&that);
  
  if ( z != NULL ) 
    {
      if ( ! m_explosed && ! z->get_hit() ) 
        explose();

      result = true;
    }
  
  return result;
} // bomb::collision_with_zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a plank.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a plank.
 */
bool rp::bomb::collision_with_plank
( bear::engine::base_item& that )
{ 
  bool result = false;
  plank* p = dynamic_cast<plank*>(&that);
  
  if ( p != NULL ) 
    {
      if ( ! m_explosed  )
        {
          if ( p->get_combo_value() != 0 )
            set_combo_value(p->get_combo_value()+1);
          explose();
        }
        
      result = true;
    }
  
  return result;
} // bomb::collision_with_plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tar.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a tar.
 */
bool rp::bomb::collision_with_tar
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {
      if ( get_mass() == s_initial_mass )
        {
          if ( t->get_current_action_name() == "idle" )
            {
              if ( get_combo_value() != 0 )
                t->set_combo_value(get_combo_value()+1);
            }
          else if ( t->get_combo_value() != 0 )
            set_combo_value(t->get_combo_value()+1);
          set_mass(s_initial_mass * 3);
          make_dirty();
          t->explose();
        }
      default_collision(info);

      result = true;
    }
  
  return result;
} // bomb::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a bird.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a bird.
 */
bool rp::bomb::collision_with_bird( bear::engine::base_item& that )
{ 
  bool result = false;
  bird* b = dynamic_cast<bird*>(&that);
  
  if ( b != NULL ) 
    {
      if ( ! m_explosed )
        {
         if ( b->get_combo_value() != 0 )
           set_combo_value(b->get_combo_value()+1);
          explose();
        }

      result = true;
    }
  
  return result;
} // bomb::collision_with_bird()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the explosition is finished.
 */
void rp::bomb::finish_explose()
{ 
  kill();
} // bomb::finish_explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the bird collides a tar.
 */
void rp::bomb::make_dirty()
{
  get_rendering_attributes().set_intensity(0,0,0);
} // bom::make_dirty()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::bomb::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::bomb, finish_explose, void );
} // bomb::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::bomb )
