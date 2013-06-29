/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the tnt class.
 * \author Sebastien Angibaud
 */
#include "rp/tnt.hpp"
#include "rp/bird.hpp"
#include "rp/bomb.hpp"
#include "rp/cannonball.hpp"
#include "rp/cart.hpp"
#include "rp/plank.hpp" 
#include "rp/explosion.hpp"
#include "rp/util.hpp"
#include "rp/zeppelin.hpp"

BASE_ITEM_EXPORT( tnt, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::tnt::tnt()
: m_explosed(false)
{
  set_system_angle_as_visual_angle(true);
} // rp::tnt::rp::tnt()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::tnt::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/tnt.cm");
  get_level_globals().load_animation("animation/explosion.canim");
} // rp::tnt::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::tnt::on_enters_layer()
{
  super::on_enters_layer();

  m_explosed = false;
  set_mass(100);
  set_model_actor( get_level_globals().get_model("model/tnt.cm") );
  start_model_action("idle");
} // rp::tnt::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::tnt::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  if ( ! collision_with_cart(that) )
    if ( ! collision_with_cannonball(that) )
      if ( ! collision_with_bird(that) )
        if ( ! collision_with_bomb(that) )
          if ( ! collision_with_obstacle(that) )
            if ( ! collision_with_plank(that) )
              if ( ! collision_with_explosion(that,info) )
                if ( ! collision_with_zeppelin(that) )
                  super::collision(that,info);
} // tnt::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the tnt exploses.
 * \param give_score Indicates if we give score.
 */
void rp::tnt::explose(bool give_score)
{ 
  if ( ! m_explosed  )
    {
      set_transportability(false);
      kill_interactive_item();

      if ( give_score ) 
        util::create_floating_score(*this,500);
      
      set_can_move_items(false);
      m_explosed = true;
      start_model_action("explose");
      set_mass(std::numeric_limits<double>::infinity());
      
      create_explosion(3,0); 
    }
} // tnt::explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::tnt::collision_with_cart( bear::engine::base_item& that )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      set_combo_value(0);          
      explose(false);
      
      result = true;
    }
  
  return result;
} // tnt::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::tnt::collision_with_cannonball
( bear::engine::base_item& that )
{ 
  bool result = false;
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL ) 
    {
      set_combo_value( c->get_combo_value() );      
      
      if ( ! m_explosed )
        c->kill();

      explose();
      result = true;
    }
  
  return result;
} // tnt::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a bird.
 */
bool rp::tnt::collision_with_bird
( bear::engine::base_item& that )
{ 
  bool result = false;
  bird* b = dynamic_cast<bird*>(&that);
  
  if ( b != NULL ) 
    {
      update_combo_value(b);      
      explose();
          
      result = true;
    }
  
  return result;
} // tnt::collision_with_bird()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::tnt::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( e != NULL ) 
    {
      if ( e->test_in_explosion(info) ) 
        {
          if ( e->get_combo_value() != 0 )
            set_combo_value(e->get_combo_value()+1);      
          explose();
        }

      result = true;
    }
  
  return result;
} // tnt::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a bomb.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a bomb.
 */
bool rp::tnt::collision_with_bomb
( bear::engine::base_item& that )
{ 
  bool result = false;
  bomb* b = dynamic_cast<bomb*>(&that);
  
  if ( b != NULL ) 
    {
      if ( ! m_explosed && ! b->is_explosed() )
        {
          update_combo_value(b);      
          explose();
          b->explose();
        }

      result = true;
    }
  
  return result;
} // tnt::collision_with_bomb()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a obstacle.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a obstacle.
 */
bool rp::tnt::collision_with_obstacle
( bear::engine::base_item& that )
{ 
  bool result = false;
  obstacle* o = dynamic_cast<obstacle*>(&that);
  
  if ( o != NULL ) 
    {
      if ( o->get_speed().length() > 200 )
        {
          update_combo_value(o);      
          explose();
          result = true;
        }
    }
  
  return result;
} // tnt::collision_with_obstacle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a plank.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a plank.
 */
bool rp::tnt::collision_with_plank
( bear::engine::base_item& that )
{ 
  bool result = false;
  plank* p = dynamic_cast<plank*>(&that);
  
  if ( p != NULL ) 
    {
      if ( p->get_combo_value() != 0 )
        set_combo_value(p->get_combo_value()+1);      
      explose();
        
      result = true;
    }
  
  return result;
} // tnt::collision_with_plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with hot air balloon.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a hot air balloon.
 */
bool rp::tnt::collision_with_zeppelin( bear::engine::base_item& that )
{ 
  bool result = false;
  zeppelin* z = dynamic_cast<zeppelin*>(&that);
  
  if ( z != NULL ) 
    {
      update_combo_value(z);      
      explose();
      
      result = true;
    }
  
  return result;
} // tnt::collision_with_zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the explosition is finished.
 */
void rp::tnt::finish_explose()
{ 
  kill();
} // tnt::finish_explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create an explosion.
 * \param nb_explosions Number of explosions to display.
 * \param radius The radius of the explosion.
 */
void rp::tnt::create_explosion
(unsigned int nb_explosions, bear::universe::coordinate_type radius)
{ 
  explosion* item = new explosion(nb_explosions,radius, 0.6);
      
  item->set_z_position(get_z_position() + 10);
  item->set_combo_value(get_combo_value());
  item->set_center_of_mass(get_center_of_mass());
  new_item( *item );
  CLAW_ASSERT( item->is_valid(),
               "The explosion of tnt isn't correctly initialized" );
} // tnt::create_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the second explosion.
 */
void rp::tnt::create_second_explosion()
{ 
  create_explosion(5,10);
  create_planks("tnt");
} // tnt::create_second_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the third explosion.
 */
void rp::tnt::create_third_explosion()
{ 
  create_explosion(6,20);
} // tnt::create_third_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::tnt::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::tnt, finish_explose, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::tnt, create_second_explosion, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::tnt, create_third_explosion, void );
} // tnt::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::tnt )
