/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the wall class.
 * \author Sebastien Angibaud
 */
#include "rp/wall.hpp"
#include "rp/cannonball.hpp"
#include "rp/cart.hpp"
#include "rp/plank.hpp" 
#include "rp/explosion.hpp"
#include "rp/tar.hpp"
#include "rp/tnt.hpp"
#include "rp/util.hpp"

#include "universe/collision_info.hpp"
#include "universe/zone.hpp"

#include <sstream>

BASE_ITEM_EXPORT( wall, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::wall::wall()
: m_top_impacts(0), m_middle_impacts(0), m_bottom_impacts(0)
{
  set_system_angle_as_visual_angle(true);
  set_can_move_items(true);
  set_phantom(false);
  set_artificial(false);
} // rp::wall::rp::wall()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::wall::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/wall.cm");

  get_level_globals().load_sound("sound/wall/break.ogg");
  get_level_globals().load_sound("sound/wall/explose.ogg");
} // rp::wall::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::wall::on_enters_layer()
{
  super::on_enters_layer();

  set_mass(10000);
  set_model_actor( get_level_globals().get_model("model/wall.cm") );
  set_size(64,123);
  start_model_action("idle");
  create_interactive_item(*this);
  init(get_top_left());
} // rp::wall::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do one step in the progression of the item.
 * \param elapsed_time Elasped time since the last progress.
 */
void rp::wall::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  update_items();
} // rp::wall::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::wall::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  if ( ! collision_with_cart(that,info) )
    {
      if ( info.get_collision_side() == bear::universe::zone::top_zone )
        {
          if ( default_collision(info) )
            collision_as_base_train(that,info);
        }
      else if ( ! collision_with_cart(that,info) )
        if ( ! collision_with_cannonball(that) )
          if ( ! collision_with_explosion(that,info) )
            if ( ! collision_with_tnt(that) )
                if ( ! collision_with_tar(that) )
                  super::collision(that,info);
    }
} // wall::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when a cannonbal hits the wall.
 * \param bottom_y The bottom y-coordinate of impact.
 * \param top_y The top y-coordinate of impact.
 */
void rp::wall::hit
( const bear::universe::coordinate_type & bottom_y, 
  const bear::universe::coordinate_type & top_y )
{
  bear::engine::model_mark_placement step1;
  bear::engine::model_mark_placement step2;

  get_level_globals().play_sound
    ( "sound/wall/break.ogg",
      bear::audio::sound_effect( get_center_of_mass() ) );

  if ( get_mark_placement("step_1", step1) )
    if ( get_mark_placement("step_2", step2) )
      {
        if ( bottom_y < step1.get_position().y ) 
          hit(& m_bottom_impacts, "bottom");
   
        if ( top_y > step2.get_position().y ) 
          hit(& m_top_impacts,"top");

        if ( ( ( top_y > step2.get_position().y ) && 
               ( bottom_y < step1.get_position().y ) ) ||
             ( ( top_y > step1.get_position().y ) && 
               ( top_y < step2.get_position().y ) ) ||
             ( ( bottom_y > step1.get_position().y ) && 
               ( bottom_y < step2.get_position().y ) ) )
          hit(& m_middle_impacts,"middle");
      }
} // wall::hit()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give a string representation of the item.
 * \param str (out) The result of the convertion.
 */
void rp::wall::to_string( std::string& str ) const
{
  super::to_string(str);
  base_train::to_string(str);
} // wall::to_string()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply the movement of the item.
 * \param elapsed_time Elasped time since the last call.
 */
void rp::wall::move( bear::universe::time_type elapsed_time )
{
  super::move(elapsed_time);

  update_item_positions(get_top_left(),get_speed());
} // wall::move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::wall::populate_loader_map( bear::engine::item_loader_map& m )
{
  super::populate_loader_map( m );

  m.insert( entity::loader( *this ) );
} // wall::populate_loader_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the items concerned by a progress/move of this one.
 * \param d (out) A list to which are added such items.
 */
void rp::wall::get_dependent_items
( bear::universe::physical_item::item_list& d ) const
{
  super::get_dependent_items(d);

  get_items(d);
} // wall::get_dependent_items()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::wall::collision_with_cart
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ( info.get_collision_side() == bear::universe::zone::top_zone ) &&
           c->get_current_action_name() != "jump" )
        c->jump();
      else if ( info.get_collision_side() != 
                bear::universe::zone::middle_zone && 
                c->get_current_action_name() != "dead" )
	{
          c->add_internal_force(bear::universe::force_type(-3000000,2000000));
          c->die_by_wall();
        }

      result = true;
    }
  
  return result;
} // wall::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::wall::collision_with_cannonball
( bear::engine::base_item& that )
{ 
  bool result = false;
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL ) 
    {
      set_combo_value( c->get_combo_value() );      
      hit(c->get_vertical_middle(),c->get_vertical_middle());
      c->kill();
      
      result = true;
    }
  
  return result;
} // wall::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::wall::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( ( e != NULL ) && 
       ( info.get_collision_side() != bear::universe::zone::middle_zone ) )
    {
      if ( e->test_in_explosion(info) ) 
        {
          if ( e->get_combo_value() != 0 )
            set_combo_value(e->get_combo_value()+1);      
          hit(e->get_bottom(),e->get_top());
        }
  
      result = true;
    }
  
  return result;
} // wall::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tnt.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a tnt.
 */
bool rp::wall::collision_with_tnt
( bear::engine::base_item& that )
{ 
  bool result = false;
  tnt* t = dynamic_cast<tnt*>(&that);
  
  if ( t != NULL ) 
    {
      if ( t->get_combo_value() != 0 )
        set_combo_value(t->get_combo_value()+1);      
      t->explose();

      result = true;
    }
  
  return result;
} // wall::collision_with_tnt()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tar.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a tar.
 */
bool rp::wall::collision_with_tar( bear::engine::base_item& that )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {
      t->explose();

      result = true;
    }
  
  return result;
} // wall::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Explose the wall.
 */
void rp::wall::explose()
{
  get_level_globals().play_sound
    ( "sound/wall/explose.ogg",
      bear::audio::sound_effect( get_center_of_mass() ) );

  util::create_floating_score(*this, 1500);

  create_decorations("bottom", 10, true);
  create_decorations("middle", 10, true);
  create_decorations("top", 10, true);

  kill();
} // wall::explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the wall receives impact.
 * \param impacts A pointer on impact counter.
 * \param name The name of the mark that receives impact.
 */
void rp::wall::hit( unsigned int * impacts, const std::string& name )
{
  (*impacts)++;
  if ( *impacts >= 3 )
    explose();
  else
    {
      std::ostringstream oss;
      oss << "broken " << name << " " << *impacts;
      set_global_substitute
        ( name, new bear::visual::animation
          ( get_level_globals().auto_sprite
            ( "gfx/wall-block/wall-block.png", oss.str() ) ) );

      create_decorations(name, 5, false);
    }
} // wall::hit()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create decorations of splinters.
 * \param name The name of the mark that receives impact.
 * \param nb_decoration The number of decoration to create.
 * \param right_orientation Indicates if decorations must have npositive speed.
 */
void rp::wall::create_decorations
( const std::string& name, unsigned int nb_decorations, bool right_orientation )
{
  bear::engine::model_mark_placement mark;
  if ( get_mark_placement(name, mark) )
    for ( unsigned int i = 0; i != nb_decorations; ++i )
      {
        bear::decorative_item* p = new bear::decorative_item();
        p->set_z_position( mark.get_depth_position() );
        p->set_center_of_mass(mark.get_position());
        p->set_mass(1);
        
      std::ostringstream stream;
      stream << "splinter " << 1 + (unsigned int)(4.0 * rand() / RAND_MAX);;
        p->set_sprite
          ( get_level_globals().auto_sprite
            ( "gfx/wall-block/wall-block.png", stream.str() ) );
        
        bear::universe::force_type force;
        if ( right_orientation )
          force.x = -50000 + 100000 * (double)rand() / RAND_MAX;
        else
          force.x = - 100000 * (double)rand() / RAND_MAX;
        force.y = 20000 + 50000 * (double)rand() / RAND_MAX;
        p->add_external_force(force);
        
        const double angular_speed = 1 + 20 * (double)rand() / RAND_MAX;
        p->set_angular_speed( angular_speed );
        
        new_item(*p);
      }
} // wall::create_decorations()
