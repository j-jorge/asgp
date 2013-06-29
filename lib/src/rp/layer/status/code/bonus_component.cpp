/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::bonus class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/bonus_component.hpp"

#include "rp/game_variables.hpp"

#include "engine/game.hpp"
#include "visual/scene_sprite.hpp"

#include <boost/bind.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/tweener_group.hpp>
#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/easing/easing_cubic.hpp>

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
rp::bonus_component::bonus_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip )
  : super(glob, active_position, side, x_p, y_p, layer_size, hide_height, flip)
{

} // bonus_component::bonus_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initializes the component.
 */
void rp::bonus_component::build()
{
  m_sprite =
    get_level_globals().auto_sprite( "gfx/status/bonus.png", "plunger" );

  m_sprite.set_intensity(0, 0, 0);
  m_sprite.set_opacity( 0 );

  super::build();
} // bonus_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::bonus_component::render( scene_element_list& e ) const
{
  bear::visual::scene_sprite s
    ( get_render_position().x + (width() - m_sprite.width()) / 2,
      get_render_position().y + (height() - m_sprite.height()) / 2, 
      m_sprite );
  e.push_back( s );
} // bonus_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Updates the state of this component.
 * \param elapsed_time The elapsed time since the last call.
 */
void rp::bonus_component::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );
  m_effect_tweener.update( elapsed_time );
} // bonus_component::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::bonus_component::width() const
{
  return m_sprite.width();
} // bonus_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::bonus_component::height() const
{
  return m_sprite.height();
} // bonus_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::bonus_component::init_signals()
{
  super::init_signals();

  add_signal
    ( bear::engine::game::get_instance().listen_uint_variable_change
      ( game_variables::get_plunger_total_number_variable_name(),
        boost::bind
        (&rp::bonus_component::on_bonus_activation_changed,
         this, _1) ) );
} // bonus_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when bonus activation changes.
 * \param value The new bonus state.
 */
void rp::bonus_component::on_bonus_activation_changed( unsigned int value )
{
  claw::tween::tweener_sequence s;

  s.insert
    ( claw::tween::single_tweener
      ( 0, 1, 0.4,
        boost::bind
        ( &rp::bonus_component::on_activation_update, this, _1 ),
        &claw::tween::easing_cubic::ease_in_out ) );

  const double angle_bound( 3.14159 / 8 );

  s.insert
    ( claw::tween::single_tweener
      ( -angle_bound, angle_bound, 0.3,
        boost::bind
        ( &bear::visual::sprite::set_angle, &m_sprite, _1 ),
        &claw::tween::easing_cubic::ease_in_out ) );
  s.insert
    ( claw::tween::single_tweener
      ( angle_bound, 0, 0.1,
        boost::bind
        ( &bear::visual::sprite::set_angle, &m_sprite, _1 ),
        &claw::tween::easing_cubic::ease_in_out ) );

  m_effect_tweener = s;
} // bonus_component::on_bonus_activation_changed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the intensity and the opacity of the sprite.
 * \param value The new intensity and opacity.
 */
void rp::bonus_component::on_activation_update( double value )
{
  m_sprite.set_intensity( value, value, value );
  m_sprite.set_opacity( value );
} // bonus_component::on_activation_update()
