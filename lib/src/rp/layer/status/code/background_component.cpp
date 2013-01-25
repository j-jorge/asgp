/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::background class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/background_component.hpp"

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
 * \param hide_height The height of hidehout gap.
 * \param flip Indicates if picture must be flipped.
 */
rp::background_component::background_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height,
  bool flip)
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height,flip)
{

} // background_component::background_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::background_component::build()
{
  m_background_sprite =
    get_level_globals().auto_sprite
    ( "gfx/status/status.png", "score background" );
  m_background_sprite.flip(is_flipped());
  //  m_background_sprite.mirror(is_flipped());

  super::build();
} // background_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::background_component::render( scene_element_list& e ) const
{
  if ( ! game_variables::is_level_ending() )
    {
      bear::visual::scene_sprite s1
        ( get_render_position().x, get_render_position().y, 
          m_background_sprite );
      
      e.push_back( s1 );
    }
} // background_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::background_component::width() const
{
  return m_background_sprite.width();
} // background_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::background_component::height() const
{
  return m_background_sprite.height();
} // background_component::height()


