/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::plunger class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/plunger_component.hpp"

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
rp::plunger_component::plunger_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip )
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height, flip)
{
} // plunger_component::plunger_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::plunger_component::build()
{
  m_sprite =
    get_level_globals().auto_sprite
    ( "gfx/status/status.png", "plunger" );
  m_background =
    get_level_globals().auto_sprite
    ( "gfx/status/status.png", "plungers background ok" );
  m_background.flip(is_flipped());

  super::build();
} // plunger_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::plunger_component::render( scene_element_list& e ) const
{
   if ( ! game_variables::is_level_ending() )
    {
      bear::visual::scene_sprite s1
        ( get_render_position().x, get_render_position().y, m_background);
      e.push_back( s1 );
      
      unsigned int total_plungers = game_variables::get_plunger_total_number();
      unsigned int plungers = game_variables::get_plunger_number();
      bool activation = game_variables::get_plunger_activation();

      bear::visual::coordinate_type gap(10);
      if ( total_plungers >= 6 )
        gap = -5;

      bear::visual::position_type pos
        ( get_render_position().x + 
          ( width() - 30 - total_plungers * m_sprite.width() - 
            ( total_plungers - 1 ) * gap ) / 2,
          get_render_position().y + ( height() - m_sprite.height() ) / 2 );

      for ( unsigned int i = 0; i < total_plungers; ++i )
        {
          bear::visual::scene_sprite s2( pos.x, pos.y, m_sprite);
          pos.x += m_sprite.width() + gap;

          if ( i >= plungers || ! activation )
            s2.get_rendering_attributes().set_intensity(0,0,0);

          e.push_back( s2 );
        }
    }
} // plunger_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::plunger_component::width() const
{
  return m_background.width();
} // plunger_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::plunger_component::height() const
{
  return m_background.height();
} // plunger_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::plunger_component::init_signals()
{
  super::init_signals();

  add_signal
    ( bear::engine::game::get_instance().listen_uint_variable_change
      ( game_variables::get_plunger_total_number_variable_name(),
        boost::bind
        (&rp::plunger_component::on_plunger_total_number_changed,
         this, _1) ) );

  add_signal
    ( bear::engine::game::get_instance().listen_uint_variable_change
      ( game_variables::get_plunger_number_variable_name(),
        boost::bind
        (&rp::plunger_component::on_plunger_number_changed,
         this, _1) ) );

  add_signal
    ( bear::engine::game::get_instance().listen_bool_variable_change
      ( game_variables::get_plunger_validity_variable_name(),
        boost::bind
        (&rp::plunger_component::on_plunger_validity_changed,
         this, _1) ) );

  add_signal
    ( bear::engine::game::get_instance().listen_bool_variable_change
      ( game_variables::get_plunger_activation_variable_name(),
        boost::bind
        (&rp::plunger_component::on_plunger_activation_changed,
         this, _1) ) );
} // plunger_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when number of total plunger changes.
 * \param value The new plunger state.
 */
void rp::plunger_component::on_plunger_total_number_changed(unsigned int value)
{
  // do nothing ? animation ?
} // plunger_component::on_plunger_total_number_changed()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when number of valid plunger changes.
 * \param value The new plunger state.
 */
void rp::plunger_component::on_plunger_number_changed(unsigned int value)
{ 
  // do nothing ? animation ?
} // plunger_component::on_plunger_number_changed()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when the validity of plunger changes.
 * \param value The new plunger validity state.
 */
void rp::plunger_component::on_plunger_validity_changed(bool value)
{
  if ( value )
    m_background =
      get_level_globals().auto_sprite
      ( "gfx/status/status.png", "plungers background ok" );
  else
    m_background =
      get_level_globals().auto_sprite
      ( "gfx/status/status.png", "plungers background out" );

  m_background.flip(is_flipped());
} // plunger_component::on_plunger_validity_changed()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when the activation of plunger changes.
 * \param value The new plunger activation state.
 */
void rp::plunger_component::on_plunger_activation_changed(bool value)
{ 
  // do nothing ? animation ?
} // plunger_component::on_plunger_activation_changed()
