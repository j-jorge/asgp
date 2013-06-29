/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the boss class.
 * \author Sebastien Angibaud
 */

#include "rp/boss.hpp"
#include "rp/cart.hpp"
#include "rp/cannonball.hpp"
#include "rp/defines.hpp"
#include "rp/explosion.hpp"
#include "rp/game_variables.hpp"
#include "rp/interactive_item.hpp"
#include "rp/plunger.hpp"
#include "rp/transition_effect/level_ending_effect.hpp"
#include "rp/util.hpp"
#include "rp/zeppelin.hpp"

#include "universe/collision_info.hpp"
#include "universe/forced_movement/forced_goto.hpp"
#include "universe/forced_movement/forced_join.hpp"
#include "universe/forced_movement/forced_sequence.hpp"
#include "universe/forced_movement/forced_tracking.hpp"

#include "engine/reference_point/model_mark_reference_point.hpp"
#include "engine/message/transition_effect_message.hpp"
#include "engine/world.hpp"

#include "generic_items/delayed_kill_item.hpp"
#include "generic_items/star.hpp"

#include <boost/bind.hpp>

#include <claw/tween/easing/easing_linear.hpp>
#include <claw/tween/easing/easing_sine.hpp>
#include <claw/tween/easing/easing_quart.hpp>
#include <claw/tween/easing/easing_back.hpp>
#include <claw/tween/single_tweener.hpp>

BASE_ITEM_EXPORT( boss, rp )

bear::universe::coordinate_type rp::boss::s_min_x_cart_distance = 400;
bear::universe::coordinate_type rp::boss::s_max_x_cart_distance = 1200;
const bear::universe::coordinate_type rp::boss::s_min_y_cart_distance = -650;
const bear::universe::coordinate_type rp::boss::s_max_y_cart_distance = 400;

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::boss::boss()
  : m_progress(NULL), m_cart(NULL), m_current_drop_item(NULL), m_open(false),
    m_module(NULL), m_module_serial(1)
{
  set_phantom(true);
  set_global(true);
  set_system_angle_as_visual_angle(true);
} // rp::boss::boss()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::boss::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/boss.cm");
  get_level_globals().load_animation("animation/boss/trap-door-closing.canim");
  get_level_globals().load_animation("animation/boss/trap-door.canim");
  get_level_globals().load_sound("sound/boss/dropping.ogg");
  get_level_globals().load_sound("sound/boss/trap-door-closing.ogg");
  get_level_globals().load_sound("sound/boss/trap-door-opening.ogg");
  get_level_globals().load_sound("sound/explosion/explosion-1.ogg");
  get_level_globals().load_sound("sound/explosion/explosion-2.ogg");
  get_level_globals().load_sound("sound/explosion/explosion-3.ogg");
  get_level_globals().load_sound("sound/explosion/explosion-4.ogg");
  get_level_globals().load_sound("sound/explosion/explosion-5.ogg");
} // rp::boss::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::boss::on_enters_layer()
{
  super::on_enters_layer();

  set_model_actor( get_level_globals().get_model("model/boss.cm") );
    
  if ( game_variables::is_boss_transition() )
    {
      if ( m_module_serial != 1 ) 
        game_variables::set_level_ending(true);
      init_transition_mode();
    }
  else
    choose_module( game_variables::get_serial_number() );
  create_anchor();
  start_model_action("fly");
  
  m_y_reference = m_cart->get_bottom();

  init_teleportation_gap();
  m_teleportation_gap = bear::universe::position_type(0,0);
  
  if ( game_variables::is_boss_transition() )
    create_transition_first_movement();
  else
    {
      create_initial_anchor_movement();
      create_initial_anchor_y_movement();
    }
    
  create_fly_movement();
  m_last_anchor_gap_x =
    m_anchor->get_vertical_middle() - m_cart->get_vertical_middle();
  m_move_on_cart = true;
  m_teleportation_gap = bear::universe::position_type(0,0);
  m_interactive_item = NULL;
  set_global_substitute
    ("emergency", new bear::visual::animation
     (get_level_globals().auto_sprite("gfx/boss/boss.png", "emergency box") ) );
} // rp::boss::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::boss::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );
  
  remove_drop_items();
  update_trap_door(elapsed_time);
  progress_tweeners(elapsed_time);
  progress_injured_state();

  if ( ! game_variables::is_boss_transition() )
    update_interactive_item();

  progress_drop_item();

  if ( m_progress != NULL )
    (this->*m_progress)(elapsed_time);

  m_last_anchor_gap_x = 
    m_anchor->get_horizontal_middle() - m_cart->get_horizontal_middle();
} // rp::boss::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c unsigned int.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::boss::set_u_integer_field
( const std::string& name, unsigned int value )
{
  bool result = true;

  if (name == "cart.module_serial")
    m_module_serial = value;  
  else
    result = super::set_u_integer_field(name, value);

  return result;
} // boss::set_u_integer_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the value of a field of type item.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::boss::set_item_field
( const std::string& name, bear::engine::base_item* value)
{
  bool result(true);

  if ( name == "boss.cart" )
    {
      m_cart = dynamic_cast<cart*>(value);
      
      if ( m_cart == NULL )
        claw::logger << claw::log_error << "boss::set_item_field:"
                     << " item is not an instance of 'rp::cart'."
                     << std::endl;
    }
  else if ( name == "boss.module" )
    m_module = value;
  else
    result = super::set_item_field(name, value);

  return result;
} // boss::set_item_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the item is well initialized.
 */
bool rp::boss::is_valid() const
{
  return m_cart != NULL;
} // boss::is_valid()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the y reference.
 * \param pos_y The new y reference.
 */
void rp::boss::set_y_reference(bear::universe::coordinate_type pos_y)
{
  m_tweener_reference_y.clear();  
  m_y_reference = pos_y;
} // boss::set_y_reference()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the gap of the last teleportation.
 * \param gap The gap of the teleportation.
 */
void rp::boss::set_teleportation_gap(const bear::universe::position_type& gap)
{
  if ( m_current_drop_item != handle_type(NULL) )
    m_teleportation_gap = gap;
} // boss::set_teleportation_gap()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the dropped item.
 * \param item The dropped item.
 * \param zeppelin_item The item that zeppelin drops.
 */
rp::boss::handle_type rp::boss::set_drop_item
(base_item* item, base_item* zeppelin_item)
{
  if ( m_current_drop_item == handle_type(NULL) && 
       get_current_action_name() != "dead" && 
       get_current_action_name() != "end" && 
       ( m_cart->get_right() < get_mark_world_position("top_item").x ||
         game_variables::is_boss_transition() ) )
    {
      base_item* const c_item = item->clone();
      c_item->set_z_position(get_z_position() - 1);
      
      zeppelin* z = dynamic_cast<zeppelin*>(c_item);
      if ( z != NULL && zeppelin_item != NULL )
        z->set_drop_item(zeppelin_item);

      m_drop_position = c_item->get_center_of_mass();
      c_item->set_top_middle(get_mark_world_position("top_item"));

      new_item( *c_item );
      bear::universe::forced_join m(1);
      m.set_reference_point
        (bear::engine::model_mark_reference_point<boss>(*this, "item"));
      m.set_moving_item_ratio(bear::universe::position_type(0.5,0));
      c_item->set_forced_movement(m);
      
      if ( z != NULL &&  zeppelin_item != NULL )
        m_drop_items.push_back(handle_type(z->get_drop_item()));

      m_current_drop_item = boss::handle_type(c_item);
      create_drop_anchor_movement();

      get_level_globals().play_sound
        ( "sound/boss/dropping.ogg", 
          bear::audio::sound_effect( get_center_of_mass() ) );

      return m_current_drop_item;
    }
  else
    return handle_type(NULL);
} // boss::set_drop_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the items concerned by a progress/move of this one.
 * \param d (out) A list to which are added such items.
 */
void rp::boss::get_dependent_items
( std::list<physical_item*>& d ) const
{
  super::get_dependent_items(d);

  if ( m_cart != NULL )
    d.push_front( m_cart );
} // boss::get_dependent_items()

/*----------------------------------------------------------------------------*/
/**
 * \brief Remove dropped items that are not in the camera.
 */
void rp::boss::remove_drop_items()
{
  if ( ! m_drop_items.empty() )
    {
      bear::universe::rectangle_type rect( get_level().get_camera_focus() );
      std::list<handle_type>::iterator it;
      std::list<handle_type>::iterator it2;

      for ( it = m_drop_items.begin(); it != m_drop_items.end(); )
        {
          it2 = it;
          ++it;
          
          if ( (*it2) != handle_type(NULL) )
            {
              if ( (*it2)->get_right() < rect.left() && 
                   ! rect.intersects((*it2)->get_bounding_box()) )
                {
                  (*it2)->kill();
                  m_drop_items.erase(it2);
                }     
            }
          else
            m_drop_items.erase(it2);         
        }
    }
} // boss::remove_drop_items();

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state fly.
 * \param elapsed_time Elapsed time since the last call.
*/
void rp::boss::progress_fly( bear::universe::time_type elapsed_time )
{
  // do nothing
} // rp::boss::progress_fly()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state dead.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::boss::progress_dead( bear::universe::time_type elapsed_time )
{
  // do nothing
} // rp::boss::progress_dead()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress the smoke in transition of serial 5.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::boss::progress_injured_state()
{
  if ( game_variables::is_boss_transition() && m_module_serial == 6 )
    {
      double d = (double)rand() / RAND_MAX;
      
      if ( d < 0.5 )
        util::create_smoke( *get_model_mark_item("cabin 2"), 1, 0.3, 0.8, -1 );
    }
} // boss::progress_injured_state()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state injure.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::boss::progress_injure( bear::universe::time_type elapsed_time )
{
  // do nothing
} // rp::boss::progress_injure()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state end.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::boss::progress_end( bear::universe::time_type elapsed_time )
{  
  if ( m_cart != NULL )
    if ( ! m_cart->can_finish() )
      {
        double d = (double)rand() / RAND_MAX;
        
        if ( d < 0.3 )
          {
            double width = (double)rand() * get_width() / RAND_MAX;
            double height = (double)rand() * get_height() / RAND_MAX;
            
            explosion* item = new explosion(2,0,0.2,true);
            item->set_z_position(get_z_position() + 10);
            item->set_center_of_mass
              ( get_left() + width/2, get_bottom() + height );
            new_item( *item );
            CLAW_ASSERT( item->is_valid(),
                         "The explosion of boss isn't correctly initialized" );
            
            bear::universe::forced_tracking m;
            m.set_reference_point_on_center(*this);
            item->set_forced_movement(m);
            
            if ( d < 0.05 )
              {
                unsigned int alea = rand() % 5 + 1;
                std::ostringstream stream;
                stream << "sound/explosion/explosion-" << alea << ".ogg";
                get_level_globals().play_sound
                  ( stream.str(), 
                    bear::audio::sound_effect(item->get_center_of_mass()) );
              }
          }
      }
} // rp::boss::progress_end()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the drop item.
 */
void rp::boss::progress_drop_item()
{
  if ( m_current_drop_item != handle_type(NULL) )
    { 
      entity* e = dynamic_cast<entity*>(m_current_drop_item.get());
  
      if ( e != NULL )
        {
          if ( ! e->is_transportable() )
            {
              m_current_drop_item->kill();
              m_current_drop_item = handle_type(NULL);
              create_fly_anchor_movement();
              create_fly_movement();
              create_initial_anchor_y_movement();
            }
          else if ( m_current_drop_item->get_left() < m_cart->get_right() && 
                    ! game_variables::is_boss_transition() )
            drop();             
        }
    }
} // progress_drop_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress the twenners.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::boss::progress_tweeners( bear::universe::time_type elapsed_time )
{
  if ( m_cart->has_bottom_contact() && 
       ( m_cart->get_bottom() < m_y_reference ) && 
       ! game_variables::is_boss_transition() )
    {
      m_tweener_reference_y = claw::tween::tweener_sequence();
      m_tweener_reference_y.insert
        ( claw::tween::single_tweener
          ( m_y_reference, m_cart->get_bottom(), 0.5, 
            boost::bind( &rp::boss::on_anchor_y_reference_change, this, _1 ), 
            &claw::tween::easing_linear::ease_in_out ) );
    }
       
  // do not change the order
  m_tweener_reference_y.update(elapsed_time);
  m_tweener_anchor_x.update(elapsed_time);
  m_tweener_anchor_y.update(elapsed_time);
  m_tweener_movement.update(elapsed_time);
  m_tweener_angle.update(elapsed_time);
} // boss::progress_tweeners()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update position of interactive_item.
 */
void rp::boss::update_interactive_item()
{
  if ( m_open )
    {
      if ( m_interactive_item == NULL )
        create_interactive_item();

      m_interactive_item->set_center_of_mass
        ( get_mark_world_position("trap corner") );
    }
  else 
    {
      bear::universe::position_type pos = get_mark_world_position("button");
      bear::universe::position_type pos_cart =
        m_cart->get_mark_world_position("plunger");
      double angle = std::atan2( pos.y - pos_cart.y, pos.x - pos_cart.x);

      if ( angle <= 0.2 )
        {
          if ( m_interactive_item == NULL )
            {
              create_interactive_item();
              set_global_substitute
                ("emergency", new bear::visual::animation
                 (get_level_globals().get_animation
                  ("animation/boss/emergency-on.canim") ) );
            }
          m_interactive_item->set_center_of_mass
            (get_mark_world_position("button"));
        }
      else if ( m_interactive_item != NULL )
        {
          m_interactive_item->kill();
          m_interactive_item = NULL;
          set_global_substitute
            ("emergency", new bear::visual::animation
             ( get_level_globals().auto_sprite
               ( "gfx/boss/boss.png", "emergency box" ) ) );
        }
    }
} // boss::update_interactive_item()
 
/*----------------------------------------------------------------------------*/
/**
 * \brief Start fly state.
 */
void rp::boss::start_fly()
{
  m_progress = &boss::progress_fly;
} // rp::boss::start_fly()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start dead state.
 */
void rp::boss::start_dead()
{
  create_dead_anchor_movement();
  create_dead_movement();
  m_progress = &boss::progress_dead;
  apply_end();
} // rp::boss::start_dead()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start injure state.
 */
void rp::boss::start_injure()
{
  m_progress = &boss::progress_injure;
} // rp::boss::start_injure()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start end state.
 */
void rp::boss::start_end()
{
  m_progress = &boss::progress_end;
} // rp::boss::start_end()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply fly.
 */
void rp::boss::apply_fly()
{
  start_model_action("fly");
} // boss::apply_fly()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply dead.
 */
void rp::boss::apply_dead()
{
  start_model_action("dead");
} // boss::apply_dead()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply injure.
 */
void rp::boss::apply_injure()
{
  start_model_action("injure");
} // boss::apply_injure()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply end.
 */
void rp::boss::apply_end()
{
  start_model_action("end");

  if ( ! game_variables::is_level_ending() )
    m_cart->apply_takeoff();
} // boss::apply_end()
    
/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the plunger.
 * \param that The other item of the collision.
 */
bool rp::boss::collision_with_plunger(bear::engine::base_item& that)
{
  bool result(false);
  
  plunger* p = dynamic_cast<plunger*>(&that);
  
  if ( p != NULL )
    {
      if ( ! p->come_back() )
        p->stop(false);
      
      result = true;
    }
  return result;
} // boss::collision_with_plunger()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the cart.
 * \param that The other item of the collision.
 */
bool rp::boss::collision_with_cart(bear::engine::base_item& that)
{
  bool result(false);
  
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL )
    {
      c->is_hit();

      result = true;
    }
  return result;
} // boss::collision_with_cart()  
  
/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the canonball.
 * \param c The cannonball.
 * \param oriented_speed Indicates orientation to give at the cannonball.
 */
void rp::boss::collision_with_cannonball
( cannonball& c, const bear::universe::speed_type& oriented_speed )
{
  c.clear_forced_movement();
  bear::universe::speed_type speed = c.get_speed();

  if ( oriented_speed.x != 0 )
    speed.x = oriented_speed.x * std::abs(speed.x);
  
  if ( oriented_speed.y != 0 )
    speed.y = oriented_speed.y * std::abs(speed.y);

  c.set_speed(speed);
  get_level_globals().play_sound
    ( "sound/hit-2.ogg", 
      bear::audio::sound_effect( c.get_center_of_mass() ) );
} // boss::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the canonball.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
bool rp::boss::collision_with_cannonball
( bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  bool result(false);
  
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL )
    {
      bear::universe::speed_type oriented_speed(0,0);

      if ( info.get_collision_side() == 
           bear::universe::zone::middle_left_zone ) 
        oriented_speed.x = -1;
      else if ( info.get_collision_side() == 
                bear::universe::zone::middle_right_zone ) 
        oriented_speed.x = 1;
      else if ( info.get_collision_side() == 
                bear::universe::zone::top_zone ) 
        oriented_speed.y = 1;
      else if ( info.get_collision_side() == 
                bear::universe::zone::bottom_zone ) 
        oriented_speed.y = -1;
      else
        {
          oriented_speed.x = -1;
          oriented_speed.y = -1;
        }

      collision_with_cannonball(*c,oriented_speed);

      result = true;
    }
  
  return result;
} // boss::collision_with_canonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the cabin.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss::on_cabin_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  if ( ! collision_with_plunger(that) )
    if ( ! collision_with_cart(that) )
      collision_with_cannonball(that,info);
} // boss::on_cabin_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the bottom of the cabin.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss::on_bottom_cabin_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  if ( ! collision_with_plunger(that) )
    if ( ! collision_with_cart(that) )
      {
        cannonball* c = dynamic_cast<cannonball*>(&that);
  
        if ( c != NULL )
          {
            if ( ! m_open || 
                 ( info.get_collision_side() != 
                   bear::universe::zone::middle_left_zone ) )
              {
                bear::universe::speed_type oriented_speed(0,-1);
                collision_with_cannonball(*c,oriented_speed);
              }
          }
      }
} // boss::on_bottom_cabin_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the left of the cabin.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss::on_left_cabin_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  if ( ! collision_with_plunger(that) )
    if ( ! collision_with_cart(that) )
      {
        cannonball* c = dynamic_cast<cannonball*>(&that);
  
        if ( c != NULL )
          {
            if ( ! m_open || 
                 ( info.get_collision_side() != 
                   bear::universe::zone::bottom_zone ) )
              {
                bear::universe::speed_type oriented_speed(-1,0);
                collision_with_cannonball(*c,oriented_speed);
              }
          }
      }
} // boss::on_left_cabin_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the trap.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss::on_trap_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  plunger* p = dynamic_cast<plunger*>(&that);
  
  if ( p != NULL )
    {
      if ( m_open )
        p->set_z_position(get_z_position() - 1);
      else if ( ! p->come_back() )
        p->stop(false);
    }
  else 
    {
      cannonball* c = dynamic_cast<cannonball*>(&that);
      
      if ( c != NULL )
        {
          if ( m_open )
            {
              c->kill();
              close_trap_door();
              hit();
            }
          else
            {
              bear::universe::speed_type oriented_speed(-1,-1);

              if ( info.get_collision_side() == 
                   bear::universe::zone::middle_left_zone ) 
                oriented_speed.y = 0;
              else if ( info.get_collision_side() == 
                        bear::universe::zone::bottom_zone ) 
                oriented_speed.x = 0;

              collision_with_cannonball(*c,oriented_speed);
            }
        }
    }
} // boss::on_trap_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the button.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss::on_button_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  plunger* p = dynamic_cast<plunger*>(&that);
  
  if ( p != NULL )
    {
      if ( ! p->come_back() )
        {
          p->stop(false);
          if ( p->get_system_angle() <= 0.2 )
            open_trap_door();
        }
    }
  else
    {
      cannonball* c = dynamic_cast<cannonball*>(&that);

      if ( c != NULL )
        {
          bear::universe::speed_type oriented_speed(-1,0);
          collision_with_cannonball(*c,oriented_speed);
        }
    }
} // boss::on_button_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the propeller.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss::on_propeller_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  if ( ! collision_with_plunger(that) )
    {
      cannonball* c = dynamic_cast<cannonball*>(&that);
      
      if ( c != NULL )
        c->kill();
    }
} // boss::on_propeller_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the emergency.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::boss::on_emergency_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  plunger* p = dynamic_cast<plunger*>(&that);
  
  if ( p != NULL )
    {
      if ( ! p->come_back() )
        p->set_z_position(get_z_position() - 1);
    }
  else
    {
      cannonball* c = dynamic_cast<cannonball*>(&that);

      if ( c != NULL )
        {
          bear::universe::speed_type oriented_speed(-1,0);
          collision_with_cannonball(*c,oriented_speed);
        }
    }
} // boss::on_emergency_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update trap duration.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::boss::update_trap_door( bear::universe::time_type elapsed_time )
{
  if ( m_open )
    {
      m_trap_duration -= elapsed_time;
      if ( m_trap_duration < 0 )
        close_trap_door();
    }
} // boss::update_trap_door()

/*----------------------------------------------------------------------------*/
/**
 * \brief Close trap door.
 */
void rp::boss::close_trap_door()
{
  m_open = false;
  set_global_substitute
    ("trap door", new bear::visual::animation
     (get_level_globals().get_animation
      ("animation/boss/trap-door-closing.canim") ) );

  get_level_globals().play_sound
    ( "sound/boss/trap-door-closing.ogg",
      bear::audio::sound_effect( get_center_of_mass() ) );
  
  update_safe_angle();
} // close_trap_door()

/*----------------------------------------------------------------------------*/
/**
 * \brief Open trap door.
 */
void rp::boss::open_trap_door()
{ 
  if ( ! m_open )
    {
      m_open = true;
      set_global_substitute
        ("trap door", new bear::visual::animation
         (get_level_globals().get_animation
          ("animation/boss/trap-door.canim") ) );

      set_global_substitute
        ("emergency", new bear::visual::animation
         (get_level_globals().auto_sprite
          ("gfx/boss/boss.png", "emergency box") ) );

      get_level_globals().play_sound
        ( "sound/boss/trap-door-opening.ogg",
          bear::audio::sound_effect( get_center_of_mass() ) );

      update_injured_angle();
    }

  m_trap_duration = 7.0;
} // boss::open_trap_door();

/*----------------------------------------------------------------------------*/
/**
 * \brief Update angle of boss.
 */
void rp::boss::update_angle()
{
  if ( ! m_open )
    update_safe_angle();
} // boss::update_angle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update angle of boss in safe state.
 */
void rp::boss::update_safe_angle()
{
  bear::universe::coordinate_type gap = 
    m_anchor->get_horizontal_middle() - m_cart->get_horizontal_middle();

  if ( gap > m_last_anchor_gap_x )
    {
      if ( m_move_on_cart )
        {
          m_tweener_angle = claw::tween::tweener_sequence();
          m_tweener_angle.insert
            ( claw::tween::single_tweener
              ( get_system_angle(), -0.05, 1.0, 
                boost::bind( &rp::boss::on_angle_change, this, _1 ), 
                &claw::tween::easing_sine::ease_in_out ) ); 
        }

      m_move_on_cart = false;
    }
 else
   {
     if ( ! m_move_on_cart )
       {
         m_tweener_angle = claw::tween::tweener_sequence();
         m_tweener_angle.insert
           ( claw::tween::single_tweener
             ( get_system_angle(), 0.05, 1.0, 
               boost::bind( &rp::boss::on_angle_change, this, _1 ), 
               &claw::tween::easing_sine::ease_in_out ) );
       }

     m_move_on_cart = true;
   }
} // boss::update_safe_angle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update angle of boss in injured state.
 */
void rp::boss::update_injured_angle()
{          
  m_tweener_angle = claw::tween::tweener_sequence();

  m_tweener_angle.insert
    ( claw::tween::single_tweener
      ( get_system_angle(), -0.06, 0.3, 
        boost::bind( &rp::boss::on_angle_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );
  m_tweener_angle.insert
    ( claw::tween::single_tweener
      ( -0.06, 0.06, 0.3, 
        boost::bind( &rp::boss::on_angle_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );

  m_tweener_angle.on_finished
    (boost::bind( &rp::boss::update_injured_angle, this ));
} // boss::update_injured_angle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create anchor.
 */
void rp::boss::create_anchor()
{
  m_anchor = new bear::reference_item;
  m_anchor->set_center_of_mass(get_center_of_mass());
  m_anchor->set_size(10,10);
  m_anchor->set_global(true);
  new_item(*m_anchor);
}// create_anchor()

/*----------------------------------------------------------------------------*/
/**
 * \brief Drop the object.
 */
void rp::boss::drop()
{
  if ( m_current_drop_item != handle_type(NULL) )
    {
      if ( game_variables::is_boss_transition() )
        restore_module();
      m_current_drop_item->set_system_angle(0);
      m_current_drop_item->clear_forced_movement();
      m_current_drop_item->set_speed(get_speed());
      m_current_drop_item->add_external_force
        ( bear::universe::force_type(0,-20000)*m_current_drop_item->get_mass());
      m_drop_items.push_back(m_current_drop_item);
      m_current_drop_item = handle_type(NULL);

      if ( game_variables::is_boss_transition() )
        {
          create_transition_third_movement();
          create_fly_movement();
        }
      else
        {
          create_fly_anchor_movement();
          create_fly_movement();
          create_initial_anchor_y_movement();
        }
    }
}// boss::drop()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create initial movement of anchor.
 */
void rp::boss::init_teleportation_gap()
{
  m_anchor->set_center_of_mass
    ( m_anchor->get_center_of_mass() + m_teleportation_gap );
  m_teleportation_gap = bear::universe::position_type(0,0);
} // boss::init_teleportation_gap()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create first movement in a transition level.
 */
void rp::boss::create_transition_first_movement()
{ 
  set_system_angle(-0.05);
  m_tweener_anchor_x = claw::tween::tweener_sequence();
  double duration = 6;

  if ( m_module_serial == 6 )
    duration = 12;

  if ( m_module_serial != 1 )
    {
      m_tweener_anchor_x.insert
        ( claw::tween::single_tweener
          ( get_horizontal_middle(), 
            get_horizontal_middle() + 2.2 * get_level().get_camera_size().x, 
            duration, 
            boost::bind( &rp::boss::on_anchor_transition_x_change, this, _1 ), 
            &claw::tween::easing_linear::ease_in_out ) );
      m_tweener_anchor_x.on_finished
        (boost::bind( &rp::boss::create_transition_second_movement, this ));
    }
  else
    m_tweener_anchor_x.insert
      ( claw::tween::single_tweener
        ( get_horizontal_middle(), 
          get_horizontal_middle(), duration, 
          boost::bind( &rp::boss::on_anchor_transition_x_change, this, _1 ), 
          &claw::tween::easing_linear::ease_in_out ) );

  m_tweener_anchor_y = claw::tween::tweener_sequence();
  m_tweener_anchor_y.insert
    ( claw::tween::single_tweener
      ( get_vertical_middle(), 
        get_vertical_middle(), duration, 
        boost::bind( &rp::boss::on_anchor_transition_y_change, this, _1 ), 
        &claw::tween::easing_linear::ease_in_out ) );
} // boss::create_transition_first_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create second movement in a transition level.
 */
void rp::boss::create_transition_second_movement()
{ 
  if ( m_module_serial != 6 )
    {
      set_system_angle(0);
      get_rendering_attributes().mirror(false);
      
      set_bottom_middle
        ( get_level().get_camera_focus().top_left() + 
          bear::universe::position_type
          ( get_level().get_camera_size().x / 2, 100) );
      
      bear::engine::model_mark_placement mark;
      bear::universe::position_type dist(0,0);
      
      if ( get_mark_placement("module", mark) && m_module != NULL )
        dist = m_module->get_center_of_mass() - mark.get_position();
      
      m_tweener_anchor_x = claw::tween::tweener_sequence();
      m_tweener_anchor_x.insert
        ( claw::tween::single_tweener
          ( get_horizontal_middle(), 
            get_horizontal_middle() + 3*dist.x / 4, 4, 
            boost::bind( &rp::boss::on_anchor_transition_x_change, this, _1 ), 
            &claw::tween::easing_quart::ease_in ) );
      m_tweener_anchor_x.insert
        ( claw::tween::single_tweener
          ( get_horizontal_middle() + 3*dist.x / 4, 
            get_horizontal_middle() + dist.x, 1.5, 
            boost::bind( &rp::boss::on_anchor_transition_x_change, this, _1 ), 
            &claw::tween::easing_sine::ease_out ) );
      m_tweener_anchor_x.on_finished
        (boost::bind( &rp::boss::start_transition_third_movement, this ));
      
      create_fly_transition_movement();
      
      m_tweener_anchor_y = claw::tween::tweener_sequence();
      m_tweener_anchor_y.insert
        ( claw::tween::single_tweener
          ( get_vertical_middle(), 
            get_vertical_middle() + dist.y, 4, 
            boost::bind( &rp::boss::on_anchor_transition_y_change, this, _1 ), 
            &claw::tween::easing_sine::ease_out ) );
      m_tweener_anchor_y.insert
        ( claw::tween::single_tweener
          ( get_vertical_middle() + dist.y, 
            get_vertical_middle() + dist.y, 1.5, 
            boost::bind( &rp::boss::on_anchor_transition_y_change, this, _1 ), 
            &claw::tween::easing_sine::ease_in_out ) );
    }
} // boss::create_transition_second_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start third movement in a transition level.
 */
void rp::boss::start_transition_third_movement()
{
  m_tweener_angle = claw::tween::tweener_sequence();
  m_tweener_angle.insert
    ( claw::tween::single_tweener
      ( get_system_angle(), 0.05, 1.0, 
        boost::bind( &rp::boss::on_angle_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) ); 
  
  restore_module();
  get_level_globals().play_sound
    ( "sound/boss/trap-door-closing.ogg", 
      bear::audio::sound_effect( get_center_of_mass() ) );

  if ( m_module != NULL )
    {
      m_module->kill();
      m_module = NULL;
    }

  create_fly_movement();
  create_transition_third_movement();
} // boss::start_transition_third_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create third movement in a transition level.
 */
void rp::boss::create_transition_third_movement()
{
  m_tweener_anchor_x = claw::tween::tweener_sequence();
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( get_horizontal_middle(), 
        get_horizontal_middle() - 2 * get_level().get_camera_size().x, 8, 
        boost::bind( &rp::boss::on_anchor_transition_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );

  m_tweener_anchor_y = claw::tween::tweener_sequence();
  m_tweener_anchor_y.insert
    ( claw::tween::single_tweener
      ( get_vertical_middle(), 
        get_vertical_middle() + get_level().get_camera_size().y / 2, 8, 
        boost::bind( &rp::boss::on_anchor_transition_y_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );
} // boss::create_transition_third_movement()
 
/*----------------------------------------------------------------------------*/
/**
 * \brief Create initial movement of anchor.
 */
void rp::boss::create_initial_anchor_movement()
{ 
  m_tweener_anchor_x = claw::tween::tweener_sequence();
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( get_horizontal_middle() - m_cart->get_left(), 
        s_min_x_cart_distance, 6, 
        boost::bind( &rp::boss::on_anchor_ratio_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( s_min_x_cart_distance, s_max_x_cart_distance, 3, 
        boost::bind( &rp::boss::on_anchor_ratio_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );
  m_tweener_anchor_x.on_finished
    (boost::bind( &rp::boss::create_fly_anchor_movement, this ));
} // boss::create_initial_anchor_movement()
 
/*----------------------------------------------------------------------------*/
/**
 * \brief Create initial movement of anchor on y-coordinate.
 ** \param initial_value The initial value;
 */
void rp::boss::create_initial_anchor_y_movement(double initial_value)
{
  init_teleportation_gap();
  
  m_tweener_anchor_y = claw::tween::tweener_sequence();
  m_tweener_anchor_y.insert
    ( claw::tween::single_tweener
      ( initial_value, 335, 4, 
        boost::bind( &rp::boss::on_anchor_ratio_y_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );  
  m_tweener_anchor_y.on_finished
    (boost::bind( &rp::boss::create_initial_anchor_y_movement, this, 335 ));
} // boss::create_initial_anchor_y_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create initial movement of anchor on y-coordinate.
 */
void rp::boss::create_initial_anchor_y_movement()
{
  create_initial_anchor_y_movement
    ( get_vertical_middle() - m_y_reference );
} // boss::create_initial_anchor_y_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create fly movement of anchor.
 */
void rp::boss::create_fly_anchor_movement()
{ 
  init_teleportation_gap();
  
  double duration = 
    2.0 * 
    std::abs( s_min_x_cart_distance + m_cart->get_left() - 
              get_horizontal_middle())
    / std::abs(s_max_x_cart_distance - s_min_x_cart_distance);

  m_tweener_anchor_x = claw::tween::tweener_sequence();
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( get_horizontal_middle() - m_cart->get_left(), 
        s_min_x_cart_distance, duration, 
        boost::bind( &rp::boss::on_anchor_ratio_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( s_min_x_cart_distance, s_max_x_cart_distance, 4, 
        boost::bind( &rp::boss::on_anchor_ratio_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );
   m_tweener_anchor_x.on_finished
    (boost::bind( &rp::boss::create_fly_anchor_movement, this ));
} // boss::create_fly_anchor_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create drop movement of anchor.
 */
void rp::boss::create_drop_anchor_movement()
{
  init_teleportation_gap();
  
  double duration = 
    std::abs(m_drop_position.x - get_mark_world_position("item").x) / 400;

  m_tweener_movement = claw::tween::tweener_sequence();
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( 0, 0, duration,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ), 
        &claw::tween::easing_sine::ease_out ) );
  m_tweener_movement.on_finished
    (boost::bind( &rp::boss::drop, this ));

  m_tweener_anchor_x = claw::tween::tweener_sequence();
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( get_horizontal_middle(), m_drop_position.x + 
        get_horizontal_middle() - get_mark_world_position("item").x, duration, 
        boost::bind( &rp::boss::on_anchor_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_out ) );

  m_tweener_anchor_y = claw::tween::tweener_sequence();
  m_tweener_anchor_y.insert
    ( claw::tween::single_tweener
      ( get_vertical_middle(), m_drop_position.y + 
        get_vertical_middle() - get_mark_world_position("item").y + 10, 
        duration, 
        boost::bind( &rp::boss::on_anchor_y_change, this, _1 ), 
        &claw::tween::easing_quart::ease_out ) );
} // boss::create_drop_anchor_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create dead movement of anchor.
 */
void rp::boss::create_dead_anchor_movement()
{
  init_teleportation_gap();
  
  m_tweener_anchor_x = claw::tween::tweener_sequence();
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( get_horizontal_middle() - m_cart->get_left(), 
        s_max_x_cart_distance, 10, 
        boost::bind( &rp::boss::on_anchor_ratio_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );

  m_tweener_anchor_y = claw::tween::tweener_sequence();
  m_tweener_anchor_y.insert
    ( claw::tween::single_tweener
      ( get_vertical_middle() - m_y_reference, 
        s_min_y_cart_distance, 10, 
        boost::bind( &rp::boss::on_anchor_ratio_y_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );  
} // boss::create_dead_anchor_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create dead movement of anchor.
 */
void rp::boss::create_dead_movement()
{
  m_tweener_movement = claw::tween::tweener_sequence();
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( 0, 40, 0.25,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ), 
        &claw::tween::easing_sine::ease_out ) );
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( 40, -20, 0.5,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ),
        &claw::tween::easing_sine::ease_in_out ) );
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( -20, 0, 0.25,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in ) );
  m_tweener_movement.on_finished
    (boost::bind( &rp::boss::create_dead_movement, this ));
} // boss::create_dead_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create final movement.
 */
void rp::boss::create_final_movement()
{
  init_teleportation_gap();
  
  m_tweener_anchor_x = claw::tween::tweener_sequence();
  m_tweener_anchor_x.insert
    ( claw::tween::single_tweener
      ( s_max_x_cart_distance, s_max_x_cart_distance, 0.2, 
        boost::bind( &rp::boss::on_anchor_ratio_x_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );

  m_tweener_anchor_y = claw::tween::tweener_sequence();
  m_tweener_anchor_y.insert
    ( claw::tween::single_tweener
      ( 20*s_min_y_cart_distance, 20*s_min_y_cart_distance, 0.2, 
        boost::bind( &rp::boss::on_anchor_ratio_y_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in_out ) );

  m_tweener_movement = claw::tween::tweener_sequence();
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( 0, 0, 0.2,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ), 
        &claw::tween::easing_sine::ease_out ) );
  m_tweener_movement.on_finished
    (boost::bind( &rp::boss::create_final_movement, this ));
} // boss::create_final_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the position of anchor on x-coordinate change.
 * \param value The ratio of current movement.
 */
void rp::boss::on_anchor_transition_x_change
(const bear::universe::coordinate_type& value)
{
  m_anchor->set_horizontal_middle( value );
} // boss::on_anchor_transition_x_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the position of anchor on y-coordinate change.
 * \param value The ratio of current movement.
 */
void rp::boss::on_anchor_transition_y_change
(const bear::universe::coordinate_type& value)
{
  m_anchor->set_vertical_middle( value );
} // boss::on_anchor_transition_y_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the ratio of anchor movement on x-coordinate 
 * change.
 * \param value The ratio of current movement.
 */
void rp::boss::on_anchor_ratio_x_change
(const bear::universe::coordinate_type& value)
{
  m_anchor->set_horizontal_middle(m_cart->get_left() + value );
  update_angle();
} // boss::on_anchor_ratio_x_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the anchor movement on x-coordinate 
 * change.
 * \param value The ratio of current movement.
 */
void rp::boss::on_anchor_x_change
(const bear::universe::coordinate_type& value)
{
  m_anchor->set_horizontal_middle(value );
  update_angle();
} // boss::on_anchor_x_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the ratio of anchor movement on y-coordinate 
 * changes.
 * \param value The ratio of current movement.
 */
void rp::boss::on_anchor_ratio_y_change
(const bear::universe::coordinate_type& value)
{
  m_anchor->set_vertical_middle( m_y_reference + value );
} // boss::on_anchor_ratio_y_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the y-coordinate reference changes.
 * \param value The new y reference.
 */
void rp::boss::on_anchor_y_reference_change
(const bear::universe::coordinate_type& value)
{
  m_y_reference = value;
} // boss::on_anchor_y_reference_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the y-coordinate changes.
 * \param value The new y reference.
 */
void rp::boss::on_anchor_y_change
(const bear::universe::coordinate_type& value)
{
  m_anchor->set_vertical_middle(value);
} // boss::on_anchor_y_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the ratio of movement change.
 * \param value The ratio of current movement.
 */
void rp::boss::on_movement_ratio_change
(const bear::universe::coordinate_type& value)
{
  set_center_of_mass
    ( m_teleportation_gap + m_anchor->get_center_of_mass() + 
      bear::universe::position_type(0,value) ); 
} // boss::on_movement_ratio_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the system angle changes.
 * \param value The new system angle.
 */
void rp::boss::on_angle_change(double value)
{
  set_system_angle(value);
  if ( m_current_drop_item != handle_type(NULL) )
    m_current_drop_item->set_system_angle(value);
} // boss::on_angle_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Create movement in fly state.
 */
void rp::boss::create_fly_movement()
{
  m_tweener_movement = claw::tween::tweener_sequence();
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( 0, 50, 0.75,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ), 
        &claw::tween::easing_sine::ease_out ) );
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( 50, -50, 1.5,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ),
        &claw::tween::easing_sine::ease_in_out ) );
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( -50, 0, 0.75,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ), 
        &claw::tween::easing_sine::ease_in ) );
  m_tweener_movement.on_finished
    (boost::bind( &rp::boss::create_fly_movement, this ));
} // boss::create_fly_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create movement in fly state in transition level.
 */
void rp::boss::create_fly_transition_movement()
{
  m_tweener_movement = claw::tween::tweener_sequence();
  m_tweener_movement.insert
    ( claw::tween::single_tweener
      ( 0, 0, 3,
        boost::bind( &rp::boss::on_movement_ratio_change, this, _1 ), 
        &claw::tween::easing_sine::ease_out ) );
  m_tweener_movement.on_finished
    (boost::bind( &rp::boss::create_fly_transition_movement, this ));
} // boss::create_fly_transition_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Choose the module.
 * \param serial The module tu put.
 */
void rp::boss::choose_module( unsigned int serial )
{ 
  std::ostringstream stream;
  stream << "gfx/boss/module-" << serial << ".png";
  
  set_global_substitute
    ("module", new bear::visual::animation
     (get_level_globals().auto_sprite(stream.str(), "module" ) ) );
} // boss::choose_module()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the boss is hitted.
 */
void rp::boss::hit()
{ 
  game_variables::set_boss_hits( game_variables::get_boss_hits() + 1);
  apply_injure();
  create_hit_star();
  
  if ( game_variables::get_boss_hits() == 1 )
    {
      throw_element("pipe 1");
      throw_element("pipe 2");
    }
  else if ( game_variables::get_boss_hits() == 2 )
    {
      throw_element("pipe 3");
      throw_element("pipe 4");
    }
  else if ( game_variables::get_boss_hits() == 3 )
    {
      throw_element("module");
      if ( m_current_drop_item != handle_type(NULL) )
        drop();

      apply_dead();
    }
} // boss::hit()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the boss is hitted.
 * \param mark_name The mark name of the model to throw.
 */
void rp::boss::throw_element(const std::string& mark_name)
{ 
  bear::engine::model_mark_placement element;
  
  if ( get_mark_placement(mark_name, element) )
    {
      bear::decorative_item* item = new bear::decorative_item;
      item->set_animation
        ( bear::visual::animation
          ( * ( get_current_action()->get_mark(mark_name).get_animation() ) ) );
      
      item->set_size(item->get_sprite().get_size());
      item->set_z_position(get_z_position()+2);
      item->set_system_angle(get_system_angle());
      item->set_system_angle_as_visual_angle(true);
      item->set_mass(10);
      item->set_angular_speed( -4 );
      item->add_external_force(bear::universe::force_type(300000, 600000));
      item->set_density(0.002);
      item->set_artificial(true);
      item->set_phantom(true);
      item->set_kill_when_leaving(true);
      item->set_center_of_mass(element.get_position());
      
      new_item( *item );
      
      CLAW_ASSERT( item->is_valid(),
                   "The decoration of boss isn't correctly initialized" );

      bear::star* star =
        new bear::star
        ( 15, 0.7, bear::visual::color_type(0,0,0), 2,
          bear::visual::color_type(220,200,0) );
      bear::universe::coordinate_type size =
        std::max(item->get_width() * 1.2, item->get_height() * 1.2 );
      star->set_size( size, size );
      star->set_z_position( item->get_z_position() - 1 );
      star->set_center_of_mass( item->get_center_of_mass() );

      new_item( *star );

      bear::universe::forced_tracking mvt
        ( bear::universe::position_type(0, 0) );
      mvt.set_reference_point_on_center( *item );
      star->set_forced_movement( mvt );

      item->kill_when_dying( *star );

      set_global_substitute(mark_name, new bear::visual::animation());      
    }
} // boss::throw_element()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the interactive_item.
 */
void rp::boss::create_interactive_item()
{ 
  m_interactive_item = new bear::reference_item();
  new_item(*m_interactive_item);
  m_interactive_item->set_size(100, 100);
  m_interactive_item->set_center_of_mass(get_mark_world_position("button"));
  entity::create_interactive_item( *m_interactive_item, 1, 0, true );
} // boss::create_interactive_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a star behing the cart.
 */
void rp::boss::create_hit_star()
{
  bear::star* s =
    new bear::star
    ( 30, 0.9, bear::visual::color_type("#E0E0E0"), 3,
      bear::visual::color_type("#DB2200") );

  bear::universe::coordinate_type size =
    std::max(get_width() * 1.2, get_height() * 1.2 );
  s->set_size(size, size);
  s->set_global(true);
  s->set_z_position( get_z_position() - 1 );
  
  new_item(*s);

  s->set_center_of_mass(get_center_of_mass());
  
  bear::universe::forced_tracking m;
  m.set_reference_point_on_center(*this);
  s->set_forced_movement(m);
  
  bear::delayed_kill_item* k = new bear::delayed_kill_item;
  k->add_item(s);
  k->set_duration(0.5);
  
  new_item(*k);
} // boss::create_hit_star()

/*----------------------------------------------------------------------------*/
/**
 * \brief Init the transition mode.
 */
void rp::boss::init_transition_mode()
{
  get_rendering_attributes().mirror(true);

  set_global_substitute( "module", new bear::visual::animation() );

  set_global_substitute( "pipe 2", new bear::visual::animation() );
  set_global_substitute( "pipe 3", new bear::visual::animation() );
  set_global_substitute( "pipe 4", new bear::visual::animation() );
} // boss::init_transition_boss()

/*----------------------------------------------------------------------------*/
/**
 * \brief Init the transition mode.
 */
void rp::boss::restore_module()
{
  choose_module( m_module_serial );

  set_global_substitute
    ( "pipe 2", new bear::visual::animation
      ( get_level_globals().get_animation("animation/boss/pipe-1.canim" ) ) );
  set_global_substitute
    ( "pipe 3", new bear::visual::animation
      ( get_level_globals().get_animation("animation/boss/pipe-3.canim" ) ) );
  set_global_substitute
    ( "pipe 4", new bear::visual::animation
      ( get_level_globals().get_animation("animation/boss/pipe-4.canim" ) ) );
} // boss::restore_module()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::boss::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::boss, start_fly, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::boss, start_dead, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::boss, start_injure, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::boss, start_end, void );
  TEXT_INTERFACE_CONNECT_METHOD_0
    ( rp::boss, create_transition_second_movement, void );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( boss, on_cabin_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( boss, on_bottom_cabin_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( boss, on_left_cabin_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( boss, on_trap_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( boss, on_button_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( boss, on_propeller_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( boss, on_emergency_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
} // boss::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::boss )
