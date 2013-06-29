/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the cursor class.
 * \author Sebastien Angibaud
 */
#include "rp/cursor.hpp"
#include "rp/entity.hpp"
#include "rp/game_variables.hpp"

#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "visual/scene_sprite.hpp"

BASE_ITEM_EXPORT( cursor, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::cursor::cursor()
{
  /*
    A mettre après la correction des courbes de bezier
  set_phantom(true);
  set_can_move_items(false);
  set_artificial(false);
  */
} // rp::cursor::cursor()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::cursor::on_enters_layer()
{
  super::on_enters_layer();  

  if ( ! m_sprite.is_valid() )
    {
      m_sprite = 
        get_level_globals().auto_sprite( "gfx/status/cursor.png", "default" );
      set_size( m_sprite.get_size() );
    }
} // cursor::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::cursor::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  m_target_position = get_center_of_mass();

  bool b = 
#if 0
    // TOP LEFT
    ( get_level().get_camera_focus().left() + 
      get_level().get_camera_size().x / 4 < 
      get_horizontal_middle() ) || 
    ( get_level().get_camera_focus().top() - 
      get_level().get_camera_size().y / 4 > 
      get_vertical_middle() );
#else
  // BOTTOM RIGHT
  ( get_level().get_camera_focus().left() + 
      3 * get_level().get_camera_size().x / 4 > 
      get_horizontal_middle() ) || 
    ( get_level().get_camera_focus().bottom() + 
      get_level().get_camera_size().y / 4 < 
      get_vertical_middle() );
#endif

  if ( b != game_variables::get_status_visibility() )
    game_variables::set_status_visibility(b);
} // cursor::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c visual::sprite.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::cursor::set_sprite_field
( const std::string& name, const bear::visual::sprite& value )
{
  bool result(true);

  if ( name == "cursor.sprite" ) 
    {
      m_sprite = value;
      set_size( m_sprite.get_size() );
    }
  else 
    result = super::set_animation_field(name,value);

  return result;
} // cursor::set_sprite_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the sprite representing the item.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::cursor::get_visual
( std::list<bear::engine::scene_visual>& visuals ) const
{
  if ( ! get_level().is_paused() && ! game_variables::get_ending_effect() )
    {
      double factor( get_level().get_camera_size().x / 1280 );

      bear::visual::position_type pos
        ( get_horizontal_middle() - m_sprite.width() * factor / 2, 
          get_vertical_middle() - m_sprite.height() * factor / 2);
      
      bear::visual::scene_sprite s( pos.x, pos.y, m_sprite);
      s.set_scale_factor(factor, factor);
      
      visuals.push_back( s );
    }
} // cursor::get_visual

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the target of the cursor.
 */
bear::universe::position_type rp::cursor::get_target() const
{
  return m_target_position;
} // rp::cursor::get_target()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision.
 * \param that The other item in the collision.
 * \param info Some informations about the collision.
 */
void rp::cursor::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  entity* e = dynamic_cast<entity*>(&that);
  
  if ( e != NULL ) 
    if ( e->has_attack_point() )
      m_target_position = e->get_attack_point();
} // cursor::collision()
