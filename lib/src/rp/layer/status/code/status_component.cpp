/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::status_component class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/status_component.hpp"
#include "rp/game_variables.hpp"

#include "engine/game.hpp"

#include <boost/bind.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_cubic.hpp>

/*----------------------------------------------------------------------------*/
const double rp::status_component::s_bar_length = 100;
const unsigned int rp::status_component::s_margin = 5;

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param glob Level globals in which we take the resources.
 * \param active_position the position in active state.
 * \param side Indicates the side on which the component appears.
 * \param x_p The placement on x-coordinate.
 * \param y_p The placement on y-coordinate.
 * \param layer_size The size of the layer.
 * \param hide_distance The height of hide gap.
 * \param flip Indicates if picture must be flipped.
 */
rp::status_component::status_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position,
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_distance, bool flip)
  : m_level_globals(glob), m_side(side),
    m_x_placement(x_p), m_y_placement(y_p),
    m_layer_size(layer_size), m_active_position(active_position),
    m_hide_distance(hide_distance), m_flip(flip), m_active( true )
{
  
} // status_component::status_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::status_component::~status_component()
{
  for ( ; !m_signals.empty(); m_signals.pop_front() )
    m_signals.front().disconnect();
} // status_component::~status_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::status_component::build()
{
  init_signals();
  update_inactive_position();
  if ( ! m_active )
    {
      on_x_position_update(m_inactive_position.x);
      on_y_position_update(m_inactive_position.y);
    }
  else
    {
      on_x_position_update(get_active_position().x);
      on_y_position_update(get_active_position().y);
    }
} // status_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the players status and update local values.
 */
void rp::status_component::progress( bear::universe::time_type elapsed_time )
{
  update_inactive_position();
  m_tweeners.update(elapsed_time);  
} // status_component::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the players status.
 * \param e (out) The scene elements.
 */
void rp::status_component::render( scene_element_list& e ) const
{
  // do nothing
} // status_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a new signal.
 * \param s The signal to add.
 */
void rp::status_component::add_signal(const boost::signals2::connection& s)
{
  m_signals.push_back(s);
} // status_component::add_signal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a new tweener.
 * \param t The tweener to add.
 */
void rp::status_component::add_tweener(const claw::tween::tweener& t)
{
  m_tweeners.insert(t);
} // status_component::add_tweener()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the current position of the component.
 */
const bear::visual::position_type& rp::status_component::get_position() const
{
  return m_position;
} // status_component::get_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the position of the component for rendering.
 */
const bear::visual::position_type& 
rp::status_component::get_render_position() const
{
  return m_render_position;
} // status_component::get_render_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the position of the component in active status.
 */
const bear::visual::position_type& 
rp::status_component::get_active_position() const
{
  return m_active_position;
} // status_component::get_active_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the position of the component in inactive status.
 */
const bear::visual::position_type& 
rp::status_component::get_inactive_position() const
{
  return m_inactive_position;
} // status_component::get_inactive_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return level globals.
 */
bear::engine::level_globals& 
rp::status_component::get_level_globals() const
{
  return m_level_globals;
} // status_component::get_level_globals()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the size of the layer.
 */
const bear::universe::size_box_type& 
rp::status_component::get_layer_size() const
{
  return m_layer_size;
} // status_component::get_layer_size()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the flip status.
 */
bool rp::status_component::is_flipped() const
{
  return m_flip;
} // status_component::is_flipped()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return if the component is active.
 */
bool rp::status_component::is_active() const
{
  return m_active;
} // status_component::is_active()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the inactive_position.
 */
void rp::status_component::update_inactive_position()
{
  m_inactive_position = m_active_position;
  m_inactive_position.x += m_hide_distance;
} // status_component::update_inactive_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the x-position of the component.
 * \param x The x-position of the component.
 */
void rp::status_component::on_x_position_update(double x)
{
  m_position.x = x;
  m_render_position.x = x;
} // status_component::on_x_position_update()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the y-position of the component.
 * \param y The y-position of the component.
 */
void rp::status_component::on_y_position_update(double y)
{
  m_position.y = y;

  m_render_position.y = y;

  if ( m_y_placement == top_placement )
    m_render_position.y -= height();
  else if ( m_y_placement == middle_y_placement )
    m_render_position.y -= (height() / 2);
} // status_component::on_y_position_update()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::status_component::init_signals()
{
  add_signal
    ( bear::engine::game::get_instance().listen_bool_variable_change
      ( game_variables::get_status_visibility_variable_name(),
        boost::bind
        (&rp::status_component::on_visibility_changed,
         this, _1) ) );
} // status_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the state of component visibility changes.
 * \param visibility Indictaes if component is visible.
 */
void rp::status_component::on_visibility_changed(bool visibility)
{
  m_tweeners.clear();

  claw::tween::single_tweener tween_x;
  
  if ( visibility )
    tween_x = claw::tween::single_tweener
      ( claw::tween::single_tweener
        (get_position().x, get_active_position().x, 0.3,
         boost::bind
         ( &rp::status_component::on_x_position_update,
           this, _1 ), &claw::tween::easing_cubic::ease_out ) );
  else
    tween_x = claw::tween::single_tweener
      ( claw::tween::single_tweener
        (get_position().x, get_inactive_position().x, 0.3,
         boost::bind
         ( &rp::status_component::on_x_position_update,
           this, _1 ), &claw::tween::easing_cubic::ease_out ) );

  add_tweener( tween_x );

  m_active = visibility;
} // status_component::on_visibility_changed()
