/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::lives class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/lives_component.hpp"

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
rp::lives_component::lives_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip )
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height, flip)
{
} // lives_component::lives_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::lives_component::build()
{
  m_sprite =
    get_level_globals().auto_sprite( "gfx/status/status.png", "hearth" );

  super::build();
} // lives_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::lives_component::render( scene_element_list& e ) const
{
   if ( ! game_variables::is_level_ending() )
    {
      unsigned int nb = game_variables::get_cart_elements_number(1);
      
      bear::visual::scene_sprite s1
        ( get_render_position().x + 2, get_render_position().y, m_sprite);
      if ( nb == 0 )
        s1.get_rendering_attributes().set_intensity(0,0,0);
      e.push_back( s1 ); 

      bear::visual::scene_sprite s2
        ( get_render_position().x + m_sprite.width() + 8, 
          get_render_position().y, m_sprite);
      if ( nb <= 1 )
        s2.get_rendering_attributes().set_intensity(0,0,0);
      e.push_back( s2 ); 

      bear::visual::scene_sprite s3
        ( get_render_position().x + m_sprite.width()/2 + 5, 
          get_render_position().y + m_sprite.height() + 4, m_sprite);
      if ( nb <= 2 )
        s3.get_rendering_attributes().set_intensity(0,0,0);
      e.push_back( s3 ); 
    }
} // lives_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::lives_component::width() const
{
  return m_sprite.width() * 2 + 6;
} // lives_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::lives_component::height() const
{
  return m_sprite.height() * 2 + 4;
} // lives_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::lives_component::init_signals()
{
  super::init_signals();

  add_signal
    ( bear::engine::game::get_instance().listen_uint_variable_change
      ( game_variables::get_cart_elements_number_variable_name(1),
        boost::bind
        (&rp::lives_component::on_lives_changed,
         this, _1) ) );
} // lives_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when number of lives changes.
 * \param value The new lives state.
 */
void rp::lives_component::on_lives_changed(unsigned int value)
{
  // do nothing ? animation ?
} // lives_component::on_lives_changed()
