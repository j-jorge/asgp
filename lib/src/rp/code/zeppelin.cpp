/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the zeppelin class.
 * \author Sebastien Angibaud
 */
#include "rp/zeppelin.hpp"
#include "rp/bird.hpp"
#include "rp/cannonball.hpp"
#include "rp/cart.hpp"
#include "rp/explosion.hpp" 
#include "rp/game_variables.hpp"
#include "rp/plank.hpp"
#include "rp/plunger.hpp"
#include "rp/tar.hpp"
#include "rp/util.hpp"

#include "universe/forced_movement/forced_rotation.hpp"
#include "universe/forced_movement/forced_tracking.hpp"
#include "universe/collision_info.hpp"
#include "universe/zone.hpp"

BASE_ITEM_EXPORT( zeppelin, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::zeppelin::zeppelin()
: m_hit(false), m_item(NULL)
{
  set_mass(100);
  set_density(0.001);
  set_system_angle_as_visual_angle(true);
  set_phantom(true);
  set_artificial(false);
} // rp::zeppelin::zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Copy constructor.
 * \param that The instance to copy from.
 */
rp::zeppelin::zeppelin( const zeppelin& that )
  : super(that), m_hit(false),
    m_item( that.m_item == NULL ? NULL : that.m_item->clone() )
{
  set_mass(100);
  set_density(0.001);
  set_system_angle_as_visual_angle(true);
  set_phantom(true);
  set_artificial(false);
} // rp::zeppelin::zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::zeppelin::~zeppelin()
{
  delete m_item;
} // rp::zeppelin::~zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::zeppelin::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/zeppelin/zeppelin.cm");
  get_level_globals().load_animation("animation/explosion.canim");
} // rp::zeppelin::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::zeppelin::on_enters_layer()
{
  super::on_enters_layer();

  if ( get_rendering_attributes().is_mirrored() )
    {
      get_rendering_attributes().mirror(false);
      set_model_actor
        ( get_level_globals().get_model("model/zeppelin/zeppelin-mirror.cm") );
    }
  else
     set_model_actor
      ( get_level_globals().get_model("model/zeppelin/zeppelin.cm") );

  start_model_action("idle");

  if ( m_item != NULL )
    create_item();

  create_interactive_item(*this, 1.5);
} // rp::zeppelin::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::zeppelin::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  if ( ! has_forced_movement() && ! game_variables::is_boss_level() && 
       ! game_variables::is_boss_transition() )
    create_default_movement();
} // zeppelin::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the value of a field of type item.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::zeppelin::set_item_field
( const std::string& name, bear::engine::base_item* value)
{
  bool result(true);

  if ( name == "zeppelin.item" )
    {
      m_item = value->clone();
      value->kill();
    }
  else
    result = super::set_item_field(name, value);

  return result;
} // zeppelin::set_item_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the item to drop.
 * \param drop_item The item to drop.
 */
void rp::zeppelin::set_drop_item( bear::engine::base_item* drop_item)
{
  delete m_item;

  if ( drop_item == NULL )
    m_item = NULL;
  else
    m_item = drop_item->clone();
} // zeppelin::set_drop_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the item to drop.
 */
bear::engine::base_item* 
rp::zeppelin::get_drop_item()
{
  if ( m_drop_item != handle_type(NULL) ) 
    return m_drop_item.get();
  else
    return NULL;
} // zeppelin::get_drop_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::zeppelin::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  if ( ! collision_with_cannonball(that) )
    if ( ! collision_with_tar(that,info) )
      if ( ! collision_with_plank(that) )
        collision_with_cart(that);
} // zeppelin::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the explosition starts.
 */
void rp::zeppelin::explose()
{ 
  game_variables::set_action_snapshot();
  set_transportability(false);
  kill_interactive_item();
  drop();
  m_hit = true;
  util::create_floating_score(*this,750);

  bear::universe::position_type pos = get_center_of_mass();
  pos.x -= get_width()/4;

  const unsigned int count( 4 );
  const double duration( 0.6 );
  
  for ( unsigned int i = 0; i != 3; ++i )
    {
      explosion* const item( new explosion( count, 20, duration ) );
      item->set_z_position(get_z_position() + 1000);
      item->set_combo_value(get_combo_value());
      item->set_center_of_mass(pos);
      new_item( *item );
      CLAW_ASSERT( item->is_valid(),
                   "The explosion of bomb isn't correctly initialized" );
      
      bear::universe::forced_tracking m;
      m.set_reference_point_on_center(*this);
      item->set_forced_movement(m);
      pos.x += get_width()/4;
    }

  start_model_action("explose");
} // zeppelin::explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return if the hot air balloon has been hit.
 */
bool rp::zeppelin::get_hit() const
{
  return m_hit;
} // zeppelin::get_hit()

/*----------------------------------------------------------------------------*/
/**
 * \brief Drop the item.
 */
void rp::zeppelin::drop()
{
  if ( m_drop_item != handle_type(NULL) )
    {
      game_variables::set_action_snapshot();
      entity* e = dynamic_cast<entity*>(m_drop_item.get());
  
      if ( e != NULL ) 
        e->set_combo_value(get_combo_value());
      
      m_drop_item->clear_forced_movement();
      m_drop_item = handle_type(NULL);
    }
} // zeppelin::drop()

/*----------------------------------------------------------------------------*/
/**
 * \brief The item is destroyed.
 */
void rp::zeppelin::destroy()
{
  if ( m_drop_item != handle_type(NULL) )
    m_drop_item->kill();

  super::destroy();
} // zeppelin::destroy()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::zeppelin::populate_loader_map( bear::engine::item_loader_map& m )
{
  super::populate_loader_map( m );

  m.insert( entity::loader( *this ) );
} // zeppelin::populate_loader_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::zeppelin::collision_with_cannonball
( bear::engine::base_item& that )
{ 
  bool result = false;
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ! m_hit )
        {
          set_combo_value( c->get_combo_value() );      
          explose();
          c->kill();
        }
      result = true;
    }
  
  return result;
} // zeppelin::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::zeppelin::collision_with_cart( bear::engine::base_item& that )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ! m_hit ) 
        {
          set_combo_value(0);      
          explose();
        }

      result = true;
    }
  
  return result;
} // zeppelin::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tar.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a tar.
 */
bool rp::zeppelin::collision_with_tar
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {
      if ( ( info.get_collision_side() == bear::universe::zone::top_zone ) && 
           ( t->get_current_action_name() != "explose" ) && 
           ( t->get_current_action_name() != "on_rail" ) )
        {
          game_variables::set_action_snapshot();
          
          if ( t->get_combo_value() != 0 )
            set_combo_value(t->get_combo_value()+1);      
          t->explose();
          drop();
          make_dirty();
        }

      default_collision(info);
      
      result = true;
    }
  
  return result;
} // zeppelin::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a plank.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a plank.
 */
bool rp::zeppelin::collision_with_plank
( bear::engine::base_item& that )
{ 
  bool result = false;
  plank* p = dynamic_cast<plank*>(&that);
  
  if ( p != NULL ) 
    {
      if ( ! m_hit  )
        { 
          if ( p->get_combo_value() != 0 )
            set_combo_value(p->get_combo_value()+1);
          explose();
        }
        
      result = true;
    }
  
  return result;
} // zeppelin::collision_with_plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the explosition is finished.
 */
void rp::zeppelin::finish_explose()
{ 
  kill();
} // zeppelin::finish_explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create item.
 */
void rp::zeppelin::create_item()
{
  base_item* const item = m_item->clone();
  
  item->set_top_middle( get_mark_world_position("anchor") );
  item->set_global( is_global() );

  entity* e = dynamic_cast<entity*>( m_item );
  if ( e != NULL )
    item->set_z_position(get_z_position() + 1);
  new_item( *item );

  m_drop_item = handle_type(item);

  bear::universe::forced_tracking mvt
    ( item->get_center_of_mass() - get_center_of_mass() );
  mvt.set_reference_point_on_center( *this );
  mvt.set_auto_remove(true);
  item->set_forced_movement( mvt );
} // zeppelin::create_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the default movement applied to the zeppelin.
 */
void rp::zeppelin::create_default_movement()
{
  bear::reference_item* center = new bear::reference_item;
  center->set_center_of_mass( get_center_of_mass() );
  
  new_item( *center );

  bear::universe::forced_rotation mvt;
  mvt.set_radius( 5 );
  mvt.set_total_time( 5 );
  mvt.set_acceleration_time( 0 );
  mvt.set_reference_point_on_center( *center );

  set_forced_movement( mvt );
} // zeppelin::()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the reppelin collides a tar.
 */
void rp::zeppelin::make_dirty()
{
  get_rendering_attributes().set_intensity(0,0,0);
} // zeppelin::make_dirty()

/*----------------------------------------------------------------------------*/
/**
 * \brief The toggle is activated, kill the player.
 * \param activator The item who activated this toggle.
 */
void rp::zeppelin::on_toggle_on
( bear::engine::base_item* activator )
{
  super::on_toggle_on(activator);

  drop();
} // zeppelin::on_toggle_on()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with screen.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::zeppelin::on_screen_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  cart* c1 = dynamic_cast<cart*>(&that);
  if ( c1 != NULL )
    collision_with_cart(*c1);
  else
    {
      cannonball* can = dynamic_cast<cannonball*>(&that);
      if ( can != NULL )
        collision_with_cannonball(*can);
      else
        {
          plunger* p = dynamic_cast<plunger*>(&that);
          if ( p != NULL )
            p->collision(*this,info);
          else
            {
              bird* b = dynamic_cast<bird*>(&that);
              if ( b != NULL )
                b->collision(*this,info); 
              else
                {
                  bear::engine::model_mark_item* m = 
                    dynamic_cast<bear::engine::model_mark_item*>(&that);
                  if ( m != NULL )
                    {
                      cart* c2 = 
                        dynamic_cast<cart*>(& (m->get_model_item() ) );
                      if ( c2 != NULL )
                        collision_with_cart(*c2);
                    }
                }
            }
        }
    }
} // zeppelin::on_screen_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::zeppelin::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( zeppelin, finish_explose, void );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( zeppelin, on_screen_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
} // zeppelin::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::zeppelin )
