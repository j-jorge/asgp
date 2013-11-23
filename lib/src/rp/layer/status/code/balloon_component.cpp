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
    m_font(glob.get_font("font/lobster.otf", 68))
{

} // balloon_component::balloon_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::balloon_component::build()
{
  std::ostringstream os;
  os << "/" << game_variables::get_required_balloons_number();
  m_required_balloon.create(m_font, os.str());
  m_required_balloon->set_intensity( 0.25, 1, 0.25 );

  on_balloon_changed(0);
  
  super::build();
} // balloon_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::balloon_component::render( scene_element_list& e ) const
{
  if ( game_variables::is_level_ending() )
    return;

  const double current_scale( height() / m_balloon.get_height() );
  const double required_scale( current_scale * 0.75 );

  const double required_left_position
    ( get_render_position().x + width()
      - m_required_balloon.get_width() * required_scale );

  bear::visual::scene_writing required
    ( required_left_position,
      get_render_position().y - 0 * ( height() * (1 - required_scale) ),
      m_required_balloon );

  required.set_scale_factor( required_scale, required_scale );

  required.set_shadow( 1, -1 );
  required.set_shadow_opacity( 0.6 );

  e.push_back( required );

  bear::visual::scene_writing current
    ( required_left_position - m_balloon.get_width() * current_scale,
      get_render_position().y,
      m_balloon );

  current.set_scale_factor( current_scale, current_scale );

  current.set_shadow( 2, -2 );
  current.set_shadow_opacity( 0.6 );

  e.push_back( current );
} // balloon_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::balloon_component::width() const
{
  return 112;
} // balloon_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::balloon_component::height() const
{
  return 34;
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

  const double required( game_variables::get_required_balloons_number() );
  const double min_intensity(0.25);

  if ( number >= required )
    m_balloon->set_intensity( min_intensity, 1, min_intensity );
  else
    m_balloon->set_intensity
      ( 1, min_intensity + ( 1 - min_intensity ) *  number / required,
      min_intensity );

  update_inactive_position();
} // balloon_component::on_balloon_changed()

