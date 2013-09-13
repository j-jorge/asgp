/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::balloon class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/balloon_component.hpp"

#include "rp/game_variables.hpp"

#include "engine/game.hpp"
#include "visual/scene_sprite.hpp"
#include "visual/bitmap_writing.hpp"
#include "visual/scene_writing.hpp"

#include <boost/bind.hpp>
#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_elastic.hpp>
#include <claw/tween/easing/easing_linear.hpp>
#include <claw/tween/easing/easing_back.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param glob Level globals in which we take the resources.
 * \param active_position the position in active state.
 * \param side Indicates the side on which the component appears.
 * \param x_p The placement on x-coordinate.
 * \param y_p The placement on y-coordinate.
 * \param layer_size The size of the layer in which the component is displayed.
 * \param flip Indicates if picture must be flipped.
 */
rp::balloon_component::balloon_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip )
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height, flip),
    m_font(glob.get_font("font/balloon/balloon.fnt",102))
{

} // balloon_component::balloon_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::balloon_component::build()
{
  std::ostringstream oss1;
  oss1 << "0";
  m_balloon.create(m_font, oss1.str());
  m_balloon->set_intensity(1, 0, 0);

  std::ostringstream oss2;
  oss2 << "/" << game_variables::get_required_balloons_number();
  m_required_balloon.create(m_font, oss2.str());
  m_required_balloon->set_intensity(0, 1, 0);
  
  m_balloon_sprite =
    get_level_globals().auto_sprite( "gfx/common.png", "balloon 0" );
  m_balloon_sprite.set_intensity(1, 0, 0);
  const double r = m_balloon_sprite.width() / m_balloon_sprite.height();
  m_balloon_sprite.set_height( 44 );
  m_balloon_sprite.set_width( m_balloon_sprite.height() * r );

  super::build();
} // balloon_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::balloon_component::render( scene_element_list& e ) const
{
  if ( ! game_variables::is_level_ending() )
    { 
      bear::universe::coordinate_type gap_x = ( 159 - width() ) / 2; 
      bear::visual::scene_sprite s1
        ( get_render_position().x + gap_x, 
          get_render_position().y, m_balloon_sprite );
      
      const double f
        ( (m_balloon_sprite.height() - s_margin) / m_balloon.get_height() );
      bear::visual::scene_writing s2
        ( get_render_position().x + gap_x + m_balloon_sprite.get_size().x + 
          s_margin + 5, get_render_position().y + 5
          + (m_balloon_sprite.height() - m_balloon_sprite.height() ) / 2,
          m_balloon);
      s2.set_scale_factor( f, f );

      bear::visual::scene_writing s3
        ( get_render_position().x + gap_x + m_balloon_sprite.get_size().x + 
          s_margin + m_balloon.get_width() * f, get_render_position().y
          + (m_balloon_sprite.height() - m_balloon_sprite.height() ) / 2,
          m_required_balloon );
      s3.set_scale_factor( f*0.5, f*0.5 );

      e.push_back( s1 );
      e.push_back( s2 );
      e.push_back( s3 );
    }
} // balloon_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::balloon_component::width() const
{
  const double f
    ( (m_balloon_sprite.height() - s_margin) / m_balloon.get_height() );
  return m_balloon_sprite.width() + s_margin + m_balloon.get_width() * f + 
    + m_required_balloon.get_width() * f * 0.5;
} // balloon_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::balloon_component::height() const
{
  return m_balloon_sprite.height();
} // balloon_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::balloon_component::init_signals()
{
  super::init_signals();

  add_signal
    ( bear::engine::game::get_instance().listen_uint_variable_change
      ( game_variables::get_balloon_variable_name(),
        boost::bind
        (&rp::balloon_component::on_balloon_changed,
         this, _1) ) );
} // balloon_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when number of balloon changes.
 * \param number The new balloon number.
 */
void rp::balloon_component::on_balloon_changed(unsigned int number)
{
  std::ostringstream oss;
  oss << number;
  m_balloon.create(m_font, oss.str());
  if ( number >= game_variables::get_required_balloons_number() )
    {
      m_balloon_sprite.set_intensity(0, 1, 0);
      m_balloon->set_intensity(0, 1, 0);
    }
  else
    {
      m_balloon_sprite.set_intensity(1, 0, 0);
      m_balloon->set_intensity(1, 0, 0);
    }

  update_inactive_position();
  
  /*
  m_balloon_sprite.set_red_intensity
    ( game_variables::get_balloon_red_intensity());
  m_balloon_sprite.set_green_intensity
    ( game_variables::get_balloon_green_intensity());
  m_balloon_sprite.set_blue_intensity
    ( game_variables::get_balloon_blue_intensity());
  */
} // balloon_component::on_balloon_changed()

