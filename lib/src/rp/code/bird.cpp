/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bird class.
 * \author Sebastien Angibaud
 */

#include "rp/bird.hpp"
#include "rp/balloon.hpp"
#include "rp/cart.hpp"
#include "rp/cannonball.hpp"
#include "rp/crate.hpp"
#include "rp/explosion.hpp"
#include "rp/cable.hpp"
#include "rp/plank.hpp"
#include "rp/tar.hpp"
#include "rp/util.hpp"
#include "rp/wall.hpp"
#include "rp/zeppelin.hpp"

#include "generic_items/explosion_effect_item.hpp"
#include "universe/collision_info.hpp"
#include "universe/zone.hpp"
#include "universe/forced_movement/forced_translation.hpp"
#include "universe/forced_movement/forced_stay_around.hpp"
#include "universe/forced_movement/forced_rotation.hpp"

BASE_ITEM_EXPORT( bird, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::bird::bird()
: m_plunger_collision(0)
{
  set_phantom(true);
} // rp::bird::bird()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::bird::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/bird.cm");
} // rp::bird::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::bird::on_enters_layer()
{
  super::on_enters_layer();

  set_model_actor( get_level_globals().get_model("model/bird.cm") );
  start_model_action("idle");

  create_interactive_item(*this);
} // rp::bird::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::bird::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  if ( ! collision_with_cannonball(that) )
    if ( ! collision_with_explosion(that,info) )
      if ( ! collision_with_cart(that) )
        if ( ! collision_with_cable(that) )
          if ( ! collision_with_wall(that) )
            if ( ! collision_with_tar(that) )
              if ( ! collision_with_balloon(that) )
                  if ( ! collision_with_crate(that, info) )
                    if ( ! collision_with_bird(that,info) )
                      if ( ! collision_with_zeppelin(that,info) )
                        collision_with_plank(that);
} // bird::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when bird is afraid.
 * \param give_points Indicates if we must give points.
 */
void rp::bird::afraid(bool give_points)
{
  if ( get_current_action_name() != "hit" && 
       get_current_action_name() != "dead" )
    {
      if ( give_points )
        util::create_floating_score(*this,50);
      
      if ( has_forced_movement() )
        clear_forced_movement();
      
      set_speed(bear::universe::speed_type(0,0));
      
      start_model_action("afraid");
    }
} // bird::afraid()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when there is a collision with plunger.
 */
void rp::bird::plunger_collision()
{
  if ( get_current_action_name() != "hit" && 
       get_current_action_name() != "dead" )
    {
      if ( get_combo_value() == 0 )
        set_combo_value(1);
      
      m_plunger_collision++; 
      util::create_floating_score(*this,50*m_plunger_collision);
      
      afraid(false);
      create_feathers();
    }
} // bird::plunger_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::bird::populate_loader_map( bear::engine::item_loader_map& m )
{
  super::populate_loader_map( m );

  m.insert( entity::loader( *this ) );
} // bird::populate_loader_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::bird::collision_with_cannonball( bear::engine::base_item& that )
{ 
  bool result = false;
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL ) 
    {
      if ( get_current_action_name() != "hit" && 
           get_current_action_name() != "dead" )
        {
          set_combo_value( c->get_combo_value() );
          start_model_action("hit");
        }

      c->kill();
      
      result = true;
    }
  
  return result;
} // bird::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with an explosion.
 */
bool rp::bird::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( e != NULL ) 
    {
      if ( ( get_current_action_name() != "hit" ) && 
           ( get_current_action_name() != "dead" ) && 
           e->test_in_explosion(info) )
        {
          if ( e->get_combo_value() != 0 )
            set_combo_value(e->get_combo_value()+1);
          start_model_action("hit");
        }
      
      result = true;
    }
  
  return result;
} // bird::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::bird::collision_with_cart( bear::engine::base_item& that )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ( get_current_action_name() != "hit" ) && 
           ( get_current_action_name() != "dead" ) )
        {
          set_combo_value(0);
          start_model_action("hit");
          c->is_hit();
        }
         
      result = true;
    }
  
  return result;
} // bird::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cable.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cable.
 */
bool rp::bird::collision_with_cable( bear::engine::base_item& that )
{ 
  bool result = false;
  cable* c = dynamic_cast<cable*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ( get_current_action_name() != "hit" ) && 
           ( get_current_action_name() != "dead" ) )
        {
          if ( c->is_ejected() && ( c->get_combo_value() != 0 ) )
            set_combo_value(c->get_combo_value()+1);
          start_model_action("hit");
        }
      
      result = true;
    }
  
  return result;
} // bird::collision_with_cable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a crate.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a crate.
 */
bool rp::bird::collision_with_crate
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  crate* c = dynamic_cast<crate*>(&that);
  
  if ( c != NULL ) 
    {
      if ( get_current_action_name() != "hit" && 
           get_current_action_name() != "dead" && 
           c->get_current_action_name() != "explose"  &&
           info.get_collision_side() != bear::universe::zone::middle_zone )
        {
          bool mirror = get_horizontal_middle() < c->get_horizontal_middle();
          
          get_rendering_attributes().mirror( mirror );
          afraid();

          if ( mirror )
            set_speed(bear::universe::speed_type(-10,0));
          else
            set_speed(bear::universe::speed_type(10,0));
        }
      
      result = true;
    }
  
  return result;
} // bird::collision_with_crate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a plank.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a plank.
 */
bool rp::bird::collision_with_plank( bear::engine::base_item& that )
{ 
  bool result = false;
  plank* p = dynamic_cast<plank*>(&that);
  
  if ( p != NULL ) 
    {
      if ( ( get_current_action_name() != "hit" ) && 
           ( get_current_action_name() != "dead" ) )
        {
          if ( p->get_combo_value() != 0 )
            set_combo_value(p->get_combo_value()+1);
          start_model_action("hit");
        }
      
      result = true;
    }
  
  return result;
} // bird::collision_with_plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a wall.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a wall.
 */
bool rp::bird::collision_with_wall( bear::engine::base_item& that )
{ 
  bool result = false;
  wall* w = dynamic_cast<wall*>(&that);
  
  if ( w != NULL ) 
    {
      get_rendering_attributes().mirror
        (! get_rendering_attributes().is_mirrored());
      if ( ( get_current_action_name() != "hit" ) && 
           ( get_current_action_name() != "dead" ) )
        start_fly();

      result = true;
    }
  
  return result;
} // bird::collision_with_wall()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tar.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a tar.
 */
bool rp::bird::collision_with_tar( bear::engine::base_item& that )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {     
      if ( ( get_current_action_name() != "hit" ) && 
           ( get_current_action_name() != "dead" ) )
        {
          if ( t->get_current_action_name() == "idle" )
            {
              if ( get_combo_value() != 0 )
                t->set_combo_value(get_combo_value()+1);
            }
          else if ( t->get_combo_value() != 0 )
            set_combo_value(t->get_combo_value()+1);
          
          start_model_action("hit");
        }
      make_dirty();
      t->kill();

      result = true;
    }
  
  return result;
} // bird::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a balloon.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a balloon.
 */
bool rp::bird::collision_with_balloon( bear::engine::base_item& that )
{ 
  bool result = false;
  balloon* b = dynamic_cast<balloon*>(&that);
  
  if ( b != NULL )
    {     
      if ( (get_current_action_name() != "dead")
           && (get_current_action_name() != "afraid") )
        {
          if ( b->get_current_action_name() != "explose" )
            b->explose();

          afraid(false);
        }

      result = true;
    }
  
  return result;
} // bird::collision_with_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a bird.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a bird.
 */
bool rp::bird::collision_with_bird
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  bird* b = dynamic_cast<bird*>(&that);
  
  if ( b != NULL ) 
    {     
      if ( ( get_current_action_name() != "hit" ) &&
           ( get_current_action_name() != "dead" ) &&
           ( info.get_collision_side() != bear::universe::zone::middle_zone) )
        {
          update_combo_value(b);
          bool mirror = get_horizontal_middle() < b->get_horizontal_middle();
          
          get_rendering_attributes().mirror( mirror );
          afraid();

          if ( mirror )
            set_speed(bear::universe::speed_type(-10,0));
          else
            set_speed(bear::universe::speed_type(10,0));
        }

      result = true;
    }
  
  return result;
} // bird::collision_with_bird()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a zeppelin.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a zeppelin.
 */
bool rp::bird::collision_with_zeppelin
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  zeppelin* z = dynamic_cast<zeppelin*>(&that);
  
  if ( z != NULL ) 
    {     
      if ( ( get_current_action_name() != "hit" ) &&
           ( get_current_action_name() != "dead" ) )
        {
          if ( z->has_forced_movement() )
            {
              z->explose();
              if ( z->get_combo_value() > 0 )
                set_combo_value(z->get_combo_value()+1);
              start_model_action("hit");
            }
          else if ( info.get_collision_side() != 
                    bear::universe::zone::middle_zone )
            {
              if ( z->get_combo_value() <= get_combo_value() )
                z->set_combo_value( get_combo_value() + 1 );
              z->drop();
              bool mirror = 
                get_horizontal_middle() < z->get_horizontal_middle();
          
              get_rendering_attributes().mirror( mirror );
              afraid();
              
              if ( mirror )
                set_speed(bear::universe::speed_type(-10,0));
              else
                set_speed(bear::universe::speed_type(10,0));
            }
        }

      result = true;
    }
  
  return result;
} // bird::collision_with_zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start fly state.
 */
void rp::bird::start_fly()
{
  bear::universe::forced_translation mvt;
  bear::universe::speed_type speed(200,40);

  if ( get_rendering_attributes().is_mirrored() )
    speed.x *= -1;
  
  mvt.set_speed(speed);  
  set_forced_movement( mvt );
} // rp::bird::start_fly()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start hit state.
 */
void rp::bird::start_hit()
{
  kill_interactive_item();
  create_explosion_feathers();
  util::create_floating_score(*this,500 + 50*m_plunger_collision);

  if ( has_forced_movement() )
    clear_forced_movement();

  set_mass(1);
  set_density(0.002);
  set_system_angle_as_visual_angle(true);
  
  if ( get_rendering_attributes().is_mirrored() )
    set_angular_speed( -4 );
  else
    set_angular_speed( 4 );
} // rp::bird::start_hit()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the bird collides a tar.
 */
void rp::bird::make_dirty()
{
  get_rendering_attributes().set_intensity(0,0,0);
} // bird::make_dirty()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the beak.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::bird::on_beak_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  balloon* b = dynamic_cast<balloon*>(&that);
  
  if ( b != NULL )
    if ( b->get_current_action_name() != "explose" )
      b->explose();
} // bird::on_beak_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create some feathers when the bird dies.
 */
void rp::bird::create_explosion_feathers()
{
  bear::explosion_effect_item* explo = new bear::explosion_effect_item;

  explo->set_size( get_size() );
  explo->set_center_of_mass( get_center_of_mass() );
  explo->set_z_position( get_z_position() + 1);
  explo->set_force(40000);
  explo->set_dust_count(0);
  explo->set_splinter_count(20);
  explo->add_splinter_animation
    (get_level_globals().auto_sprite("gfx/bird/bird.png", "feather, brown"));
  explo->add_splinter_animation
    (get_level_globals().auto_sprite("gfx/bird/bird.png", "feather, white"));

  new_item( *explo );
  CLAW_ASSERT( explo->is_valid(),
               "The explosion isn't correctly initialized" );
} // bird::create_explosion_feathers()
  
/*----------------------------------------------------------------------------*/
/**
 * \brief Create some feathers when the bird is hitted by plunger.
 */
void rp::bird::create_feathers()
{
  for ( unsigned int i = 0; i != 5; ++i )
    create_feather();
} // bird::create_explosion_feathers()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a feather when the bird dies.
 */
void rp::bird::create_feather()
{
  // create anchor_reference
  bear::reference_item* ref = new bear::reference_item;
  set_feather_anchor_reference(ref);
  new_item( *ref );
  CLAW_ASSERT( ref->is_valid(),
               "The reference anchor of feather of bird isn't correctly "
               "initialized" );

  // create reference
  bear::reference_item* ref2 = new bear::reference_item;
  set_feather_reference(ref2);
  new_item( *ref2 );
  CLAW_ASSERT( ref2->is_valid(),
                 "The reference of bird isn't correctly initialized" );

  // create movement
  bear::universe::forced_stay_around mvt;
  mvt.set_max_angle( 0.4 );
  mvt.set_speed( 120 );
  mvt.set_max_distance( 200 );
  mvt.set_apply_angle( true );
  mvt.set_reference_point_on_center(*ref);
  mvt.set_item(*ref2);
  ref2->set_forced_movement( mvt );

  // create feather
  bear::decorative_item* item = new bear::decorative_item;
  set_feather(item);
  new_item( *item );
  CLAW_ASSERT( item->is_valid(),
                 "The feather of bird isn't correctly initialized" );

  // create movement
  bear::universe::forced_rotation mvt2;
  mvt2.set_radius( 50.0 );
  mvt2.set_acceleration_time(0.2);
  double alea = (double)rand() / RAND_MAX;
  mvt2.set_start_angle( -2.17 + alea * 0.3);
  mvt2.set_end_angle( -0.97 - alea * 0.3);
  mvt2.set_loop_back(true);
  mvt2.set_total_time(1.0 + alea * 0.3);
  mvt2.set_reference_point_on_center(*ref2);
  mvt2.set_auto_remove(true);
  mvt2.set_angle_application
    (bear::universe::forced_rotation::apply_force_delta);
  mvt2.set_item(*item);
  item->set_forced_movement( mvt2 );
} // bird::create_feather()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the reference item for feather anchor movement.
 * param ref The item to set.
 */
void rp::bird::set_feather_anchor_reference
(bear::reference_item* ref)
{
  ref->set_phantom(true);
  ref->set_artificial(true);
  ref->set_can_move_items(true);
  ref->set_mass(0.001);
  double alea = (double)rand() / RAND_MAX;
  ref->set_friction(0.6 + alea * 0.2);

  double r1=(double)rand()/(double)RAND_MAX;
  double r2=(double)rand()/(double)RAND_MAX;
  bear::universe::force_type force(300*(r1-0.5), 250*(r2-0.5));
  ref->add_external_force(force);

  ref->set_height(1);
  ref->set_width(1);
  ref->set_center_of_mass
    (get_center_of_mass() + bear::universe::position_type(0,50));
} // bird::set_feather_anchor_reference()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the reference item for feather movement.
 * param ref The item to set.
 */
void rp::bird::set_feather_reference( bear::reference_item* ref)
{
  ref->set_phantom(true);
  ref->set_artificial(true);
  ref->set_can_move_items(true);

  ref->set_height(1);
  ref->set_width(1);
  ref->set_center_of_mass
    (get_center_of_mass() + bear::universe::position_type(0,50));
} // bird::set_feather_reference()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the feather.
 * param item The item to set.
 */
void rp::bird::set_feather(bear::decorative_item* item)
{
  bear::engine::level_globals& glob = get_level_globals();

  item->set_friction(0.7);

  double r = 2* ((double)rand() / (double)RAND_MAX);
  if ( r >= 1 )
    item->set_sprite
      (glob.auto_sprite("gfx/bird/bird.png", "feather, brown"));
  else
    item->set_sprite
      (glob.auto_sprite("gfx/bird/bird.png", "feather, white"));

  item->set_phantom(true);
  item->set_artificial(true);
  item->set_can_move_items(false);
  item->get_rendering_attributes().set_angle(-0.5);
  item->set_kill_on_contact(false);
  item->set_z_position(get_z_position()+1);
  item->set_system_angle_as_visual_angle(true);
  item->set_mass(0.001);
  item->set_center_of_mass
    (get_center_of_mass() + bear::universe::position_type(0,50));
} // bird::set_feather()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::bird::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::bird, start_fly, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::bird, start_hit, void );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( rp::bird, on_beak_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
} // bird::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::bird )
