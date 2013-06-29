/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::interactive_item class.
 * \author Sebastien Angibaud
 */
#include "rp/interactive_item.hpp"
#include "rp/cursor.hpp"
#include "rp/entity.hpp"
#include "rp/game_variables.hpp"

#include "engine/level.hpp"
#include "engine/item_brick/with_rendering_attributes.hpp"
#include "generic_items/star.hpp"
#include "universe/forced_movement/forced_tracking.hpp"
#include "generic_items/decorative_effect.hpp"

#include "visual/scene_sprite.hpp"
#include "visual/scene_writing.hpp"

#include <boost/bind.hpp>
#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_linear.hpp>

BASE_ITEM_EXPORT( interactive_item, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Contructor.
 */
rp::interactive_item::interactive_item()
: m_item(NULL), m_sprite_factor(1), m_area_factor(0.5), 
  m_display_star(true)
{
  set_artificial(false);
  set_phantom(true);
} // interactive_item::interactive_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Contructor.
 * \param item The item to follow.
 * \param sprite_factor The factor of sprites.
 * \param area_factor The factor of required area for activation.
 * \param display_star Indicates that we display always the star.
 * \param gap The gap according to the center of mass. 
 */
rp::interactive_item::interactive_item
(bear::engine::base_item* item, double sprite_factor, double area_factor,
 bool display_star, bear::universe::position_type gap)
  : m_item( item ), m_sprite_factor(sprite_factor), m_area_factor(area_factor),
    m_display_star(display_star), m_gap(gap)
{
  set_artificial(false);
  set_phantom(true);
  set_system_angle_as_visual_angle(true);
} // interactive_item::interactive_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Loads the media required by this class.
 */
void rp::interactive_item::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_sound("sound/effect/over.ogg");
} // interactive_item::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::interactive_item::on_enters_layer()
{
  super::on_enters_layer();

  m_cursor_in_collision = false;
  
  m_cannonball_sprite =
    get_level_globals().auto_sprite
    ( "gfx/effect/rounded-star-bright.png", "star" );
  m_cannonball_sprite.colorize( bear::visual::color( "#ffff27" ) );

  m_plunger_sprite =
    get_level_globals().auto_sprite
    ( "gfx/effect/light-star-dark.png", "star" );
  m_plunger_sprite.colorize( bear::visual::color( "#77d623" ) );

  bear::visual::sprite spr
    ( get_level_globals().auto_sprite( "gfx/effect/light-star.png", "star" ) );
  spr.colorize( bear::visual::color( "#fffbda" ) );
  set_sprite( spr );

  deactivate();
  
  if ( m_item != handle_type(NULL) )
    {
      update_item();
      
      bear::universe::forced_tracking m;
      m.set_reference_point_on_center( *m_item );
      m.set_distance(m_gap); 
      set_forced_movement(m);
    }
} // interactive_item::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::interactive_item::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );
  m_tweeners.update(elapsed_time);  

  if ( m_item != handle_type(NULL) )
    {  
      m_cannonball_sprite.set_opacity(0);
      m_plunger_sprite.set_opacity(0);

      if ( m_cursor_in_collision && ! m_activated )
        activate();
      else if ( ! m_cursor_in_collision && m_activated )
        deactivate();      
      
      bool visible = true;
      if ( m_activated )
        {
          if ( game_variables::get_cannonball_activation() && 
               game_variables::get_cannonball_validity() )
            {
              visible = false;
              m_cannonball_sprite.set_opacity(1);
            }
          
          if ( game_variables::get_plunger_activation() && 
               game_variables::get_plunger_validity() )
            {
              visible = false;
              m_plunger_sprite.set_opacity(1);
            }
        }

      if ( visible )
        get_rendering_attributes().set_opacity(1);
      else
        get_rendering_attributes().set_opacity(0);

      set_system_angle( get_system_angle() + m_angular_speed * elapsed_time );
      m_plunger_sprite.set_angle( get_system_angle() );
      update_item();
    }
  else
    kill();

  m_cursor_in_collision = false;
} // interactive_item::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the sprite representing the item.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::interactive_item::get_visual
( std::list<bear::engine::scene_visual>& visuals ) const
{
  bool visible(true);

  if ( m_item != handle_type(NULL) )
    {
      entity* e = dynamic_cast<entity*>( m_item.get() );
      if ( e != NULL )
        visible = ! e->get_passive();
    }

  if ( visible )
    {
      super::get_visual(visuals);
      
      // plunger star
      bear::visual::position_type pos2
        ( get_horizontal_middle() - (double)m_plunger_sprite.width() / 2.0,
          get_vertical_middle() - (double)m_plunger_sprite.height() / 2.0 );
      bear::visual::scene_sprite s2( pos2.x, pos2.y, m_plunger_sprite);
      
      visuals.push_back( s2 );
      
      // cannonball star
      bear::visual::position_type pos1
        ( get_horizontal_middle() - (double)m_cannonball_sprite.width() * 
          m_cannonball_factor / 2.0,
          get_vertical_middle() - (double)m_cannonball_sprite.height() * 
          m_cannonball_factor / 2.0 );
      bear::visual::scene_sprite s1( pos1.x, pos1.y, m_cannonball_sprite);
      s1.set_scale_factor(m_cannonball_factor,m_cannonball_factor);
      visuals.push_back( s1 );  
    }
} // interactive_item::get_visuals();

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::interactive_item::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);
  
  cursor* c = dynamic_cast<cursor*>(&that);
  
  if ( c != NULL ) 
    {
      bear::universe::coordinate_type min_area =
        std::min(c->get_bounding_box().area(), get_bounding_box().area());

      m_cursor_in_collision = 
        c->get_bounding_box().intersection(get_bounding_box()).area() > 
        min_area * m_area_factor;
    }
} // interactive_item::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Activate the item.
 */
void rp::interactive_item::activate()
{
  get_level_globals().play_sound
    ( "sound/effect/over.ogg",
      bear::audio::sound_effect( get_center_of_mass() ) );

  m_activated = true;
  m_angular_speed = 1;
  m_cannonball_factor = 1.0;

  claw::tween::tweener_sequence tween;
  tween.insert
    ( claw::tween::single_tweener
      (1.0, 3.0, 0.2,
       boost::bind
       ( &rp::interactive_item::on_cannonball_factor_change,
         this, _1 ), &claw::tween::easing_linear::ease_out ) );
  tween.insert
    ( claw::tween::single_tweener
      (3.0, 2.0, 0.2,
       boost::bind
       ( &rp::interactive_item::on_cannonball_factor_change,
         this, _1 ), &claw::tween::easing_linear::ease_out ) );
  
  m_tweeners.insert(tween);
} // interactive_item::activate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Deactivate the item.
 */
void rp::interactive_item::deactivate()
{ 
  m_activated = false;
  m_angular_speed = 0.4;
  m_cannonball_factor = 1.0;
  m_cannonball_sprite.set_opacity(0);
  m_plunger_sprite.set_opacity(0);
} // interactive_item::deactivate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Activate the item.
 * \param factor The new factor of cannonball sprite
 */
void rp::interactive_item::on_cannonball_factor_change( double factor )
{
  m_cannonball_factor = factor;
} // interactive_item::on_cannonball_factor_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Compute the size of the interactive item.
 */
bear::universe::coordinate_type rp::interactive_item::compute_size() const
{  
  if ( m_item != handle_type(NULL) )
    return 
      std::max
      ( 58.0, 
        std::min( m_item->get_size().x, m_item->get_size().y ) * 
        m_sprite_factor );
  else
    return 0;
} // interactive_item::compute_size()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update parameters of interactive item.
 */
void rp::interactive_item::update_item()
{  
  set_z_position( m_item->get_z_position() - 1 );
  bear::universe::coordinate_type size = compute_size();
  m_cannonball_sprite.set_size(size,size);

  if ( m_activated )
    size *= 3;
  else
    size *= 2;

  m_plunger_sprite.set_size(size,size);
  set_size(m_item->get_size());
  get_animation().set_size(size,size);
  set_gap_x( (get_width() - size) / 2);
  set_gap_y( (get_height() - size) / 2);
  set_center_of_mass(m_item->get_center_of_mass() + m_gap);
} // interactive_item::update_item()
