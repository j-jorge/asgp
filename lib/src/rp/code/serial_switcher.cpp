/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the serial_switcher class.
 * \author Sebastien Angibaud
 */
#include "rp/serial_switcher.hpp"
#include "rp/game_variables.hpp"
#include "rp/util.hpp"

#include "engine/level_globals.hpp"

#include "visual/scene_sprite.hpp"

#include <boost/bind.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_linear.hpp>

BASE_ITEM_EXPORT( serial_switcher, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::serial_switcher::serial_switcher()
: m_serial(1), m_next_serial(1), m_mouse_in(false), m_angle(0)
{
} // rp::serial_switcher::rp::serial_switcher()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::serial_switcher::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_image("gfx/status/level/frame-2.png");
} // rp::serial_switcher::pre_cache()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::serial_switcher::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  progress_input_reader(elapsed_time);
  m_angle_tweener.update( elapsed_time );

  m_animation_on.next(elapsed_time);
  m_animation_off.next(elapsed_time);

  if ( ( game_variables::is_demo_version() && m_next_serial <= 1 
         && m_serial <= 1 ) ||
       ( ! game_variables::is_demo_version() && 
         game_variables::get_unlocked_serial() >= m_next_serial ) )
    get_rendering_attributes().set_opacity(1);
  else
    get_rendering_attributes().set_opacity(0);
} // serial_swticher::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c unsigned int.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::serial_switcher::set_u_integer_field
( const std::string& name, unsigned int value )
{
  bool result = true;

  if (name == "serial_switcher.next_serial")
    m_next_serial = value;  
  else if (name == "serial_switcher.serial")
    m_serial = value;
  else
    result = super::set_u_integer_field(name, value);

  return result;
} // serial_switcher::set_u_integer_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c visual::animation.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::serial_switcher::set_animation_field
( const std::string& name, const bear::visual::animation& value )
{
  bool result(true);

  if ( name == "serial_switcher.animation_off" ) 
    m_animation_off = value;
  else if ( name == "serial_switcher.animation_on" ) 
    m_animation_on = value;
  else 
    result = super::set_animation_field(name,value);

  return result;
} // serial_switcher::set_animation_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the sprite representing the item.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::serial_switcher::get_visual
( std::list<bear::engine::scene_visual>& visuals ) const
{
  super::get_visual(visuals);
  
  if ( is_visible() )
    {
      bear::universe::position_type gap(0,0);

      if ( m_mouse_in )
        {
          bear::visual::scene_sprite s
            ( get_left() + gap.x, get_bottom() + gap.y, 
              m_animation_on.get_sprite() );
          s.get_rendering_attributes().set_opacity
            ( get_rendering_attributes().get_opacity() );
          if ( m_serial != 0 )
            s.get_rendering_attributes().set_angle( m_angle );
          visuals.push_back( s );
        }
      else
        {
          bear::visual::scene_sprite s
            ( get_left() + gap.x , get_bottom() + gap.y, 
              m_animation_off.get_sprite() );
          s.get_rendering_attributes().set_opacity
            ( get_rendering_attributes().get_opacity() );
          visuals.push_back( s );
        }
    }
} // serial_switcher::get_visuals()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the player to stop the action associated with a mouse button.
 * \param button The code of the button.
 * \param pos The position of the cursor on the screen.
 */
bool rp::serial_switcher::mouse_released
( bear::input::mouse::mouse_code button,
  const claw::math::coordinate_2d<unsigned int>& pos )
{
  bool result = false;

  if ( is_visible() )
    {
      result = super::mouse_released(button, pos);

      if ( result && m_serial != 6 && m_serial != 0 )
        game_variables::set_last_serial( m_serial );
         
      if ( result )
        game_variables::set_selected_serial( m_next_serial );
    }
  else
    result = false;

  return result;
} // serial_switcher::mouse_released

/*----------------------------------------------------------------------------*/
/**
 * \brief Method called when the position of the mouse changes.
 * \param pos The new position of the cursor.
 * \return true if the event has been processed.
 */
bool rp::serial_switcher::mouse_move
( const claw::math::coordinate_2d<unsigned int>& pos )
{
  bool result= false;

  bool last_mouse_in = m_mouse_in; 

  m_mouse_in =
    get_bounding_box().includes( get_level().screen_to_level(pos) );

  if ( m_mouse_in && ! last_mouse_in )
    create_angle_tweener();

  return result;
} // serial_switcher::mouse_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the tweener for angle.
 */
void rp::serial_switcher::create_angle_tweener()
{
  m_angle_tweener = claw::tween::tweener_sequence();
  m_angle_tweener.insert
    ( claw::tween::single_tweener
      ( 0, 0.2, 0.2, boost::bind
        ( &rp::serial_switcher::on_angle_change, this, _1 ), 
        &claw::tween::easing_linear::ease_in ) ); 
  m_angle_tweener.insert
    ( claw::tween::single_tweener
      (0.2, -0.2, 0.4, boost::bind
        ( &rp::serial_switcher::on_angle_change, this, _1 ), 
       &claw::tween::easing_linear::ease_in ) ); 
  m_angle_tweener.insert
    ( claw::tween::single_tweener
      (-0.2, 0, 0.2, 
       boost::bind( &rp::serial_switcher::on_angle_change, this, _1 ), 
       &claw::tween::easing_linear::ease_in ) );

  m_angle_tweener.on_finished
    (boost::bind( &rp::serial_switcher::create_angle_tweener, this ));
} // serial_switcher::create_angle_tweener()

/*----------------------------------------------------------------------------*/
/**
 * \brief The angle changes.
 * \param angle The new angle/
 */
void rp::serial_switcher::on_angle_change( double angle )
{
  m_angle = angle;
} // serial_switcher::on_angle_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Test if the item is visible.
 */
bool rp::serial_switcher::is_visible() const
{
  if ( game_variables::is_demo_version() )
    {
      game_variables::set_selected_serial(1);
      return ( m_next_serial <= 1 && m_serial <= 1 );
    }
  else
    return
      ( game_variables::get_unlocked_serial() >= m_next_serial ) &&
      ( m_serial != 6 || 
        game_variables::get_last_serial() == m_next_serial ) &&
      ( m_serial != 0 || 
        game_variables::get_selected_serial() == m_next_serial );
} // serial_switcher::is_visible()
