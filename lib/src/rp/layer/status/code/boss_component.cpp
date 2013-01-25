/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::boss class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/boss_component.hpp"

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
rp::boss_component::boss_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip )
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height, flip)
{

} // boss_component::boss_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::boss_component::build()
{
  m_sprite =
    get_level_globals().auto_sprite( "gfx/boss/lady.png", "lady middle" );

  const double r = m_sprite.width() / m_sprite.height();
  m_sprite.set_height( 44 );
  m_sprite.set_width( m_sprite.height() * r );

  super::build();
} // boss_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::boss_component::render( scene_element_list& e ) const
{
  if ( ! game_variables::is_level_ending() )
    {
      unsigned int nb = game_variables::get_boss_hits();
      bear::visual::position_type pos = get_render_position();

      for ( unsigned int i = 0; i != 3; i++ )
        {
          bear::visual::scene_sprite s( pos.x, pos.y, m_sprite);
          if ( nb > i )
            s.get_rendering_attributes().set_intensity(0,0,0);
          e.push_back( s ); 
          
          pos.x += m_sprite.width() + s_margin;
        }
    }
} // boss_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::boss_component::width() const
{
  return m_sprite.width() * 3 + 2 * s_margin;
} // boss_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::boss_component::height() const
{
  return m_sprite.height(); 
} // boss_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::boss_component::init_signals()
{
  super::init_signals();

  add_signal
    ( bear::engine::game::get_instance().listen_uint_variable_change
      ( game_variables::get_boss_hits_variable_name(),
        boost::bind
        (&rp::boss_component::on_boss_changed,
         this, _1) ) );
} // boss_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when number of hits on boss changes.
 * \param number The new boss hits count.
 */
void rp::boss_component::on_boss_changed(unsigned int hits_count)
{
  // do nothing ? animation ?
} // boss_component::on_boss_changed()

