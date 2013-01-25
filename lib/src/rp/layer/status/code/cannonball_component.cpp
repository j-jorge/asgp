/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::cannonball class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/cannonball_component.hpp"

#include "rp/game_variables.hpp"

#include "engine/game.hpp"
#include "visual/scene_sprite.hpp"
#include "visual/bitmap_writing.hpp"
#include "visual/scene_writing.hpp"

#include <boost/bind.hpp>
#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_elastic.hpp>
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
rp::cannonball_component::cannonball_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip )
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height, flip)
{
} // cannonball_component::cannonball_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::cannonball_component::build()
{
  m_sprite =
    get_level_globals().auto_sprite
    ( "gfx/status/status.png", "cannon" );
  m_background =
    get_level_globals().auto_sprite
    ( "gfx/status/status.png", "cannon background ok" );
  m_background.flip(is_flipped());

  super::build();
} // cannonball_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::cannonball_component::render( scene_element_list& e ) const
{
   if ( ! game_variables::is_level_ending() )
    {
      bear::visual::scene_sprite s1
        ( get_render_position().x, get_render_position().y, m_background);
      bear::visual::scene_sprite s2
        ( get_render_position().x + (width() - m_sprite.width())/2,
          get_render_position().y + (height() - m_sprite.height())/2, 
          m_sprite);
      e.push_back( s1 );
      e.push_back( s2 );
    }
} // cannonball_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::cannonball_component::width() const
{
  return m_background.width();
} // cannonball_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::cannonball_component::height() const
{
  return m_background.height();
} // cannonball_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::cannonball_component::init_signals()
{
  super::init_signals();

  add_signal
    ( bear::engine::game::get_instance().listen_bool_variable_change
      ( game_variables::get_cannonball_activation_variable_name(),
        boost::bind
        (&rp::cannonball_component::on_cannonball_activation_changed,
         this, _1) ) );
  add_signal
    ( bear::engine::game::get_instance().listen_bool_variable_change
      ( game_variables::get_cannonball_validity_variable_name(),
        boost::bind
        (&rp::cannonball_component::on_cannonball_validity_changed,
         this, _1) ) );
} // cannonball_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when cannonball activation changes.
 * \param value The new cannonball state.
 */
void rp::cannonball_component::on_cannonball_activation_changed(bool value)
{
  if ( value )
    m_sprite.set_intensity(1,1,1);
  else
    m_sprite.set_intensity(0,0,0);
} // cannonball_component::on_cannonball_activation_changed()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when cannonball validity changes.
 * \param value The new cannonball state.
 */
void rp::cannonball_component::on_cannonball_validity_changed(bool value)
{
  if ( value )
    m_background =
      get_level_globals().auto_sprite
      ( "gfx/status/status.png", "cannon background ok" );
  else
    m_background =
      get_level_globals().auto_sprite
      ( "gfx/status/status.png", "cannon background out" );

  m_background.flip(is_flipped());
} // cannonball_component::on_cannonball_validity_changed()
