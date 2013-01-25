/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the plunger class.
 * \author Sebastien Angibaud
 */
#include "rp/plunger.hpp"
#include "rp/attractable_item.hpp"
#include "rp/bird.hpp"
#include "rp/cart.hpp"
#include "rp/game_variables.hpp"
#include "rp/obstacle.hpp"
#include "rp/wall.hpp"
#include "rp/zeppelin.hpp"

#include "engine/reference_point/model_mark_reference_point.hpp"
#include "engine/item_brick/item_with_input_listener.hpp"
#include "engine/item_brick/item_with_toggle.hpp"
#include "engine/base_item.hpp"

#include "universe/forced_movement/forced_translation.hpp"
#include "universe/forced_movement/forced_tracking.hpp"
#include "universe/forced_movement/forced_join.hpp"
#include "universe/forced_movement/forced_goto.hpp"
#include "universe/forced_movement/forced_sequence.hpp"

BASE_ITEM_EXPORT( plunger, rp )

const double rp::plunger::s_max_plunger_distance = 1000;
const double rp::plunger::s_max_boss_plunger_distance = 1300;

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::plunger::plunger()
  : m_come_back(false), m_attracted_item(NULL), m_angle(0)
{
  set_phantom(true);
  set_system_angle_as_visual_angle(true);
  set_size(36,22);
} // rp::plunger::plunger()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::plunger::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_model("model/plunger.cm");
} // rp::plunger::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::plunger::on_enters_layer()
{
  super::on_enters_layer();

  if ( game_variables::is_boss_level() )
    m_max_plunger_distance = s_max_boss_plunger_distance;
  else
    m_max_plunger_distance = s_max_plunger_distance;
  
  set_model_actor( get_level_globals().get_model("model/plunger.cm") );
  start_model_action("idle");
  m_camera_rect = get_level().get_camera_focus();
} // rp::plunger::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::plunger::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );
  
  update_angle();
  
  if ( m_cart != NULL )
    {
      bear::engine::model_mark_placement plunger_mark;

      if ( m_cart->get_mark_placement("plunger", plunger_mark) )
         {
           bear::universe::vector_type vect
             ( get_tail_position() - plunger_mark.get_position() );

           if ( ( vect.length() > m_max_plunger_distance || 
                  ! m_camera_rect.intersects
                  ( get_bounding_box() ) ) && ! m_come_back )
             {
               create_back_movement(false);
               game_variables::set_bad_plunger_number
                 ( game_variables::get_bad_plunger_number() + 1);
             }
           else if ( m_come_back && 
                     ( ! has_forced_movement() ||
                       vect.length() < 20 ) )
             {
               m_cart->finish_plunger(this, m_attracted_item);
               m_cart = NULL;
               kill();
             }
         }
    }
} // plunger::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set cart.
 * \param c The cart.
 */
void rp::plunger::set_cart( cart* c )
{
  m_cart = c;
} // plunger::set_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get cart.
 */
rp::cart* rp::plunger::get_cart()
{
  return m_cart;
} // plunger::get_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief The attractable object leaves the plunger.
 */
void rp::plunger::leave()
{
  m_attracted_item = handle_type(NULL);
} // plunger::leave()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get tail position.
 */
bear::universe::position_type rp::plunger::get_tail_position() const
 {
   bear::engine::model_mark_placement body_mark;

   if ( get_mark_placement("body", body_mark) )
     {
       bear::universe::position_type pos =
         body_mark.get_position();
       
       pos.x += -17 * cos(m_angle);
       pos.y += -17 * sin(m_angle);
       
       return pos;
     }
  else
    return bear::universe::position_type();
 } // plunger::get_tail_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create movement of the object.
 * \param dir Direction of the fire.
 * \param cart_speed Speed of the cart.
 */
void rp::plunger::create_movement
( const bear::universe::vector_type& dir,
  const bear::universe::speed_type& cart_speed)
{
  bear::universe::forced_translation mvt;
  
  bear::universe::vector_type speed(dir);  
  speed.normalize();
  speed *= (1300 + cart_speed.length());
  
  mvt.set_speed(speed);
  mvt.set_auto_remove(true);
  set_forced_movement( mvt );
} // plunger::create_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Stop the plunger.
 * \param bounce Indicates if there is a bounce movement.
 */
void rp::plunger::stop( bool bounce )
{
  create_back_movement(bounce);
} // plunger::stop()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get if the plunger is in come back state.
 */
bool rp::plunger::come_back() const
{
  return m_come_back;
} // plunger::come_back()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::plunger::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
 if ( ! collision_with_attractable(that) )
    if ( ! collision_with_zeppelin(that) )
      if ( ! collision_with_bird(that) )
        if ( ! collision_with_wall(that) )
          collision_with_obstacle(that);
} // plunger::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 */
bool rp::plunger::collision_with_attractable( bear::engine::base_item& that )
{
  bool result = false;
  attractable_item* item = dynamic_cast<attractable_item*>(&that);

  if ( item != NULL ) 
    {
      if ( ( m_attracted_item == handle_type(NULL) ) && ! m_come_back && 
           item->is_attractable() && ! item->get_taken_state() )
        {
          create_back_movement(false);

          m_attracted_item = handle_type(item);
          item->set_z_position(get_z_position() - 1); 
          item->set_combo_value(1);
          item->attract(this);
        }
      
      result = true;
    }

  return result;
} // plunger::collision_with_attractable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a zeppelin.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a zeppelin.
 */
bool rp::plunger::collision_with_zeppelin
( bear::engine::base_item& that )
{ 
  bool result = false;
  zeppelin* z = dynamic_cast<zeppelin*>(&that);
  
  if ( z != NULL ) 
    {
      if ( ! m_come_back && z->get_current_action_name() != "explose" )
        {
          create_back_movement(true);
          z->set_combo_value(1);
          z->drop();
        }

      result = true;
    }
  
  return result;
} // plunger::collision_with_zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a bird.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a bird.
 */
bool rp::plunger::collision_with_bird( bear::engine::base_item& that )
{ 
  bool result = false;
  bird* b = dynamic_cast<bird*>(&that);
  
  if ( b != NULL ) 
    {
      if ( ! m_come_back )
        {
          create_back_movement(true);
          b->plunger_collision();
        }

      result = true;
    }
  
  return result;
} // plunger::collision_with_bird()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an obstacle.
 * \param that The other item of the collision.
 * \return Return True if the collision is with an obstacle.
 */
bool rp::plunger::collision_with_obstacle
( bear::engine::base_item& that )
{ 
  bool result = false;
  obstacle* o = dynamic_cast<obstacle*>(&that);
  
  if ( o != NULL ) 
    {
      if ( ! m_come_back && ( o->get_current_action_name() != "explose" ) )
        {
#if 0
          attract_cart(that);
#else
          create_back_movement(true);
#endif
        }

      result = true;
    }
  
  return result;
} // plunger::collision_with_obstacle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a wall.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a wall.
 */
bool rp::plunger::collision_with_wall( bear::engine::base_item& that )
{ 
  bool result = false;
  wall* w = dynamic_cast<wall*>(&that);
  
  if ( w != NULL ) 
    {
      if ( ! m_come_back )
#if 0
        attract_cart(that);
#else
      create_back_movement(true);
#endif

      result = true;
    }
  
  return result;
} // plunger::collision_with_wall()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create attracted movement of the object.
 * \param that The item of the collision.
 */
void rp::plunger::create_attracted_movement(bear::engine::base_item& that)
{
  m_come_back = true;

  bear::universe::position_type dist
    (get_center_of_mass() - that.get_center_of_mass());

  bear::universe::forced_sequence mvt;
  mvt.set_auto_remove(true);
  mvt.set_auto_angle(false);
  mvt.set_loops(1);

  bear::universe::forced_tracking mvt1;
  mvt1.set_distance(dist);
  mvt1.set_reference_point_on_center(that);
  mvt1.set_total_time(0.4);
  mvt1.set_auto_angle(false);
  
  bear::universe::forced_join mvt2;
  mvt2.set_total_time(0.5);
  mvt2.set_auto_angle(false);
  mvt2.set_reference_point
    ( bear::engine::model_mark_reference_point<cart>(*m_cart, "plunger") );

  mvt.push_back(mvt1);
  mvt.push_back(mvt2);

  set_forced_movement( mvt );
} // plunger::create_attracted_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create back movement of the object.
 * \param bounce Indicates if there is a bounce movement.
 */
void rp::plunger::create_back_movement(bool bounce)
{
  m_come_back = true;
  
  if ( m_cart != NULL )
    {
      bear::engine::model_mark_placement plunger_mark;      
      
      bear::universe::forced_sequence mvt; 
      bear::universe::forced_goto mvt1; 
      bear::universe::forced_goto mvt2;       
      bear::universe::forced_join mvt3;

      if ( m_cart->get_mark_placement("plunger", plunger_mark) )
        {
          bear::universe::vector_type vect
            (get_tail_position() - plunger_mark.get_position());
          mvt3.set_total_time(vect.length() * 1 / m_max_plunger_distance);
          vect.normalize();
          vect *= -30;
          mvt1.set_length( vect );
        }
      else
        {
          mvt1.set_length(bear::universe::vector_type(20,0));
          mvt3.set_total_time(1);
        }
      
      mvt1.set_total_time(0.2);
      mvt1.set_acceleration_time(0);
      mvt2.set_length(bear::universe::vector_type(0,0));          
      mvt2.set_total_time(0.2);
      mvt2.set_acceleration_time(0);
      mvt3.set_auto_angle(false);      
      mvt3.set_reference_point
        ( bear::engine::model_mark_reference_point<cart>(*m_cart, "plunger") );
      
#if 0
      if ( bounce )
        {
          mvt.push_back(mvt1);
          mvt.push_back(mvt2);
        }
#endif
      mvt.push_back(mvt3);
      mvt.set_auto_remove(true);
      mvt.set_loops(1);
      set_forced_movement( mvt );
    }
} // plunger::create_back_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with plunger.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::plunger::on_plunger_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  collision(that,info);
} // plunger::on_plunger_collision()

/*---------------------------------------------------------------------------*/
/**
 * \brief Update angle of the item.
 */
void rp::plunger::update_angle()
{
  if ( m_cart != NULL )
    {
      bear::engine::model_mark_placement plunger_mark;

      if ( m_cart->get_mark_placement("plunger", plunger_mark) )
        {
          const bear::universe::vector_type vect
            (get_center_of_mass() - plunger_mark.get_position());
            
          m_angle = std::atan2( vect.y, vect.x );
          set_system_angle(m_angle);
        }
    }
} // plunger::update_angle()

/*---------------------------------------------------------------------------*/
/**
 * \brief Update position of the plunger.
 */
void rp::plunger::update_plunger_position
(bear::universe::time_type elapsed_time) 
{
  bear::engine::model_mark_placement plunger_mark;
  bear::engine::model_mark_placement body_mark;
  
  if ( get_mark_placement("plunger", plunger_mark) && 
       get_mark_placement("body", body_mark) )
    {
      bear::universe::position_type pos = body_mark.get_position();

      pos.x += 14 * cos(m_angle);
      pos.y += 14 * sin(m_angle);

      pos += elapsed_time * get_speed();
      
      get_model_mark_item("plunger")->set_center_of_mass(pos);
    }
} // plunger::update_plunger_position()

/*---------------------------------------------------------------------------*/
/**
 * \brief Attract the cart.
 * \param that The item that attracts the cart.
 */
void rp::plunger::attract_cart(bear::engine::base_item& that)
{
  create_attracted_movement(that);

  if ( m_cart != NULL )
    m_cart->add_internal_force(bear::universe::force_type(6000000, 0));
} // plunger::attract_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::plunger::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( plunger, on_plunger_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
} // plunger::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::plunger )
