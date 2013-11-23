/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::level_starting_effect class.
 * \author Sébastien Angibaud
 */
#include "rp/transition_effect/level_starting_effect.hpp"
#include "rp/game_variables.hpp"
#include "rp/rp_gettext.hpp"
#include "rp/util.hpp"

#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "visual/scene_rectangle.hpp"
#include "visual/scene_sprite.hpp"
#include "visual/scene_writing.hpp"
#include "visual/scene_polygon.hpp"
#include "visual/scene_rectangle.hpp"

#include "rp/defines.hpp"

#include <algorithm>

#include <boost/bind.hpp>

#include <claw/tween/easing/easing_sine.hpp>
#include <claw/tween/easing/easing_linear.hpp>
#include <claw/tween/easing/easing_back.hpp>
#include <claw/tween/single_tweener.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::level_starting_effect::level_starting_effect()
  : m_visibility_duration(4.5), m_start_time(1.5), m_elapsed_time(0), 
    m_counter_duration(1.5), m_nb_balloons(0), m_balloons_scale(1),
    m_factor_intensity(0), m_fade_in_opacity(1.0), m_fade_out_duration(0.5),
    m_level_name_gap_y(0), m_panel_gap_y(0)
{

} // level_starting_effect::level_starting_effect()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the effect is finished.
 */
bool rp::level_starting_effect::is_finished() const
{
  return m_elapsed_time >= m_visibility_duration + m_fade_out_duration;
} // level_starting_effect::is_finished()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialise the effect.
 */
void rp::level_starting_effect::build()
{
  create_controls();
  m_tweener_level_name_gap_y = claw::tween::tweener_sequence();
  m_tweener_level_name_gap_y.insert
    ( claw::tween::single_tweener
      ( m_level_name_gap_y, 0, 0.5, 
        &claw::tween::easing_back::ease_in ) );
  
  m_tweener_panel_gap_y = claw::tween::tweener_sequence();
  m_tweener_panel_gap_y.insert
    ( claw::tween::single_tweener
      ( m_panel_gap_y, 0, 0.5, 
        &claw::tween::easing_back::ease_in ) );
} // level_starting_effect::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adjust the components of the effect.
 * \param elapsed_time Elapsed time since the last call.
 */
bear::universe::time_type
rp::level_starting_effect::progress( bear::universe::time_type elapsed_time )
{
  bear::universe::time_type result(0);

  if ( ! get_level().is_paused() )
    {
      if ( m_elapsed_time < m_visibility_duration + m_fade_out_duration )
        {
          m_text.update( elapsed_time );

          if ( m_elapsed_time > m_start_time )
            update_balloons_number();
          
          if ( ( m_elapsed_time < m_start_time + m_counter_duration ) &&
               ( m_elapsed_time + elapsed_time >= 
                 m_start_time + m_counter_duration ) )
            create_balloons_tweener();
            
          m_elapsed_time += elapsed_time;

          if ( m_elapsed_time >= m_visibility_duration || 
               game_variables::is_boss_level() )
            if ( ! game_variables::level_has_started() )
                end_effect();

          if ( m_elapsed_time > m_visibility_duration )
            result = m_elapsed_time - m_visibility_duration;

          m_tweener_scale.update(elapsed_time);
          m_tweener_fade_in.update(elapsed_time);
        }
    
      m_tweener_level_name_gap_y.update(elapsed_time);
      m_tweener_panel_gap_y.update(elapsed_time);
    }

  return result;
} // level_starting_effect::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the components of the effect.
 * \param e (out) The scene elements.
 */
void rp::level_starting_effect::render( scene_element_list& e ) const
{
  bear::visual::coordinate_type y_panel;
  const bear::visual::position_type center
        ( get_layer().get_size().x / 2, get_layer().get_size().y / 2);

  if ( ! get_level().is_paused() && ! game_variables::is_boss_level() )
    {      
      if ( m_elapsed_time <= m_visibility_duration + m_fade_out_duration )
        {
          bear::visual::coordinate_type y_panel = render_panel(e,center);
          render_balloon_text(e,center, y_panel);
        }
    }

  if ( m_fade_in_opacity > 0 )
    render_opaque_rectangle(e);

  if ( ! game_variables::is_boss_transition() )
    render_level_name(e, center, y_panel);
} // level_starting_effect::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the level name.
 * \param e (out) The scene elements.
 * \param center The center of the layer.
 * \param y_panel The y-coordinate of panel.
 */
void  rp::level_starting_effect::render_level_name
( scene_element_list& e, const bear::visual::position_type& center, 
  const bear::visual::coordinate_type& y_panel) const
{
  bear::universe::rectangle_type rect
    ( bear::universe::position_type(0,0), get_layer().get_size() );

  bear::universe::position_type pos( util::get_level_name_position( rect ) );
  
  bear::visual::scene_writing result
    ( pos.x - m_level_name.get_width() / 2, 
      pos.y - m_level_name.get_height() / 2 + m_level_name_gap_y, 
      m_level_name );

  result.set_shadow( RP_LEVEL_NAME_SHADOW, -RP_LEVEL_NAME_SHADOW );
  result.set_shadow_opacity( 1 );

  e.push_back( result );
} // level_starting_effect::render_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the panel.
 * \param e (out) The scene elements.
 * \param center The center of the layer.
 */
bear::visual::coordinate_type rp::level_starting_effect::render_panel
( scene_element_list& e, const bear::visual::position_type& center) const
{
  bear::visual::scene_sprite panel
    ( center.x - m_background.width() / 2,
      center.y - m_background.height() / 2 + m_panel_gap_y, m_background );

  panel.set_shadow( 10, -10 );
  panel.set_shadow_opacity( 1 );

  e.push_back( panel );

  return panel.get_position().y;
} // level_starting_effect::render_panel()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the opaque rectangle.
 * \param e (out) The scene elements.
 */
void rp::level_starting_effect::render_opaque_rectangle
( scene_element_list& e) const
{
  bear::universe::rectangle_type rect
    ( bear::universe::position_type(0,0),
      bear::universe::position_type
      (get_layer().get_size().x, get_layer().get_size().y) );

  bear::visual::scene_element e1
    ( bear::visual::scene_rectangle
      ( 0,0,
        claw::graphic::black_pixel, rect, true) );
  e1.get_rendering_attributes().set_opacity(m_fade_in_opacity);

  e.push_back( e1 );
} // level_starting_effect::render_opaque_rectange()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render balloon text.
 * \param e (out) The scene elements.
 * \param center The center of the layer.
 * \param y_panel The y-coordinate of panel.
 */
void rp::level_starting_effect::render_balloon_text
( scene_element_list& e, const bear::visual::position_type& center,
  const bear::visual::coordinate_type& y_panel) const
{
  const bear::visual::coordinate_type balloons_bottom
    ( y_panel + m_background.height() - m_balloons.get_height() / 2);

  bear::visual::scene_writing balloons_text
    ( bear::visual::scene_writing
      ( center.x - m_balloons.get_width() * m_balloons_scale / 2, 
        y_panel + m_background.height()
      - m_balloons.get_height() * m_balloons_scale / 2, 
        m_balloons) );
 
  balloons_text.get_rendering_attributes().set_intensity
    ( std::min(1.0, 2.0 - 2.0 * m_factor_intensity), 
      std::min(1.0, 2.0 * m_factor_intensity), 0);
  balloons_text.set_scale_factor( m_balloons_scale, m_balloons_scale );
  e.push_back( balloons_text );

  const double border_width( 15 );

  const double scale_factor
    ( std::min
      ( (m_background.width() - 2 * border_width) / m_text.get_width(),
        (balloons_bottom - y_panel - 2 * border_width)
        / m_text.get_height() ) );

  bear::visual::scene_writing text
    ( bear::visual::scene_writing
      ( center.x - m_text.get_width() * scale_factor / 2, 
        y_panel + border_width, m_text) );

  text.set_scale_factor( scale_factor, scale_factor );
  e.push_back( text );
} // level_starting_effect::render_balloon_text

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the controls of the interface.
 */
void rp::level_starting_effect::create_controls()
{
  const bear::visual::font level_name_font
    ( get_level_globals().get_font("font/fontopo/fontopo.fnt",50) );

  m_level_name.create
    ( level_name_font, util::get_level_name(),
      bear::visual::text_align::align_center );

  if ( ! game_variables::is_boss_level() )
    {
      const bear::visual::font main_font
        ( get_level_globals().get_font("font/balloon/balloon.fnt", 102) );
      
      m_text.create
        ( main_font, rp_gettext("Balloons\nto go!"),
          bear::visual::text_align::align_center );

      bear::visual::sequence_effect effect;
      effect.set_wave_length( 8 );
      effect.set_wave_height( main_font.get_line_spacing() / 2 );
      effect.set_wave_speed( 1 );
      effect.set_decrease_coefficient( main_font.get_line_spacing() / 8 );
      
      effect.add_color( claw::graphic::blue_pixel );
      effect.add_color( claw::graphic::green_pixel );
      effect.add_color( claw::graphic::red_pixel );
      effect.add_color( claw::graphic::yellow_pixel );
      effect.add_color( claw::graphic::magenta_pixel );
      effect.add_color( claw::graphic::cyan_pixel );
      effect.add_color( bear::visual::color_type( "#ffc900" ) );
      
      m_text.set_effect( effect );

      m_balloons.create( main_font, "0" );

      m_background = 
        bear::visual::sprite
        ( get_level_globals().auto_sprite( "gfx/status/intro.png", "panel" ) );
    }

  create_fade_in_tweener();
} // level_starting_effect::create_controls()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the text containing balloons number.
 */
void rp::level_starting_effect::update_balloons_number()
{
  std::ostringstream oss;
  unsigned int balloon_count =
    std::min
    ( game_variables::get_required_balloons_number(),
      (unsigned int)
      ( game_variables::get_required_balloons_number()
        * (m_elapsed_time - m_start_time) / m_counter_duration ) );
  
  if ( (balloon_count != m_nb_balloons) || (m_elapsed_time == 0) )
    {
      m_nb_balloons = balloon_count;
      oss << m_nb_balloons;

      m_balloons.create
        ( get_level_globals().get_font("font/balloon/balloon.fnt", 102),
          oss.str() );
      m_factor_intensity = 
        (double)m_nb_balloons / 
        (double)game_variables::get_required_balloons_number();
    }
} // level_starting_effect::update_balloons_number()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create tweener for balloons writing scale.
 */
void rp::level_starting_effect::create_balloons_tweener()
{
  get_level_globals().play_sound( "sound/effect/tiny-bell.ogg" );

  m_balloons_scale = 1;
  m_tweener_scale = claw::tween::tweener_sequence();
  m_tweener_scale.insert
    ( claw::tween::single_tweener
      ( m_balloons_scale, 2, 0.5, 
        &claw::tween::easing_sine::ease_in_out ) );
  m_balloons_scale = 2;
  m_tweener_scale.insert
    ( claw::tween::single_tweener
      ( m_balloons_scale, 1, 0.5, 
        &claw::tween::easing_sine::ease_in_out ) );
} // level_starting_effect::create_balloons_tweener()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create tweener for fade in opacity
 */
void rp::level_starting_effect::create_fade_in_tweener()
{
  m_tweener_fade_in = 
    claw::tween::single_tweener
    ( m_fade_in_opacity, 0.0, 1.0, 
      &claw::tween::easing_linear::ease_in_out );
} // level_starting_effect::create_fade_in_tweener()

/*----------------------------------------------------------------------------*/
/**
 * \brief End of the effect
 */
void rp::level_starting_effect::end_effect()
{
  if ( game_variables::is_boss_level() )
    m_fade_out_duration += 2.0;
  
  game_variables::set_level_starting(true);

  m_tweener_level_name_gap_y = claw::tween::tweener_sequence();
  if ( game_variables::is_boss_level() )
    m_tweener_level_name_gap_y.insert
      ( claw::tween::single_tweener
        ( m_level_name_gap_y,0, 2.0, 
          &claw::tween::easing_linear::ease_in ) );
  m_tweener_level_name_gap_y.insert
    ( claw::tween::single_tweener
      ( m_level_name_gap_y, get_level().get_camera_focus().size().y / 2, 0.5, 
        &claw::tween::easing_back::ease_in ) );
  
  m_tweener_panel_gap_y = claw::tween::tweener_sequence();
  if ( game_variables::is_boss_level() )
    m_tweener_panel_gap_y.insert
      ( claw::tween::single_tweener
        ( m_panel_gap_y, 0, 2.0, 
          &claw::tween::easing_linear::ease_in ) );
  m_tweener_panel_gap_y.insert
    ( claw::tween::single_tweener
      ( m_panel_gap_y, - get_level().get_camera_focus().size().y, 0.5, 
        &claw::tween::easing_linear::ease_in ) );
} // level_starting_effect::end_effect()
