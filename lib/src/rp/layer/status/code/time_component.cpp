/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::time_component class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/time_component.hpp"

#include "rp/game_variables.hpp"

#include "engine/game.hpp"

#include "visual/bitmap_writing.hpp"
#include "visual/scene_writing.hpp"

#include <libintl.h>

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
 * \param t The timer to use.
 */
rp::time_component::time_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position, 
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip,
  const timer_handle& t )
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height, flip),
    m_font(glob.get_font("font/fontopo/fontopo.fnt",50)),
    m_timer(t)
{

} // time_component::score_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::time_component::build()
{
  m_text_time.create(m_font, "00:00");
  
  super::build();
} // time_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the players status and update local values.
 */
void rp::time_component::progress
( bear::universe::time_type elapsed_time )
{
  super::progress(elapsed_time); 

  if ( m_timer != (bear::timer*)NULL )
    {
      m_time_on = true;

      std::ostringstream stream;
      stream <<  (unsigned int)m_timer->get_time() << gettext("s");
      m_text_time.create( m_font, stream.str() );
    }
  else
    m_time_on = false;
}// time_component::progress

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the players oxygen.
 * \param e (out) The scene elements.
 */
void rp::time_component::render( scene_element_list& e ) const
{ 
  if ( m_time_on )
    {
      e.push_back
        ( bear::visual::scene_writing
          (get_render_position().x,
           get_render_position().y, m_text_time));
    }
} // time_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::time_component::width() const
{
  return m_text_time.get_width();
} // time_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::time_component::height() const
{
  return m_text_time.get_height();
} // time_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 * \param t The timer to use.
 */
void rp::time_component::set_timer( const timer_handle& t )
{
  m_timer = t;
} // time_component::set_timer()
