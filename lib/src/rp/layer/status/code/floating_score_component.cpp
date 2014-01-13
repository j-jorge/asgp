/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::floating_score_component class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/floating_score_component.hpp"

#include "rp/game_variables.hpp"

#include "visual/scene_sprite.hpp"
#include "visual/scene_writing.hpp"

#include <boost/bind.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/symmetric_easing.hpp>
#include <claw/tween/easing/easing_quad.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief This easing function is applied to the floating_score_component for
 *        a fun movement.
 * \param t The ratio of the time at which the easing is computed during the
 *        tweener.
 */
double easing_back_large_func( double t )
{
  if ( t <= 0.0045 )
    return 0;
  else
    return 1 - 2.5 * (1 - t*t*t*t) * std::log( 0.8 * t + 0.1 ) / log(0.8 * t);
} // easing_back_large_func()

/*----------------------------------------------------------------------------*/
/**
 * \brief Implementation of the in/out/in_out versions of the
 *        easing_back_large_func easing function.
 */
typedef claw::tween::symmetric_easing<easing_back_large_func> easing_back_large;

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param glob The level globals.
 */
rp::floating_score_component::floating_score_component
( bear::engine::level_globals& glob )
{
  m_combo_value = game_variables::get_combo();
  m_points_value = game_variables::get_points();

  std::ostringstream oss;
  if ( m_combo_value < 6 )
    oss << "combo " << m_combo_value;
  else
    oss << "combo 6";
  
  m_sprite = 
    glob.auto_sprite( "gfx/status/score.png", oss.str() );
  
  std::ostringstream os;
  os << m_points_value;
  m_points.create( glob.get_font("font/beroga.ttf", 14), os.str() );
  
  os.str( std::string() );
  os.clear();
  
  if ( m_combo_value > 1 )
    os << m_combo_value;

  m_combo.create( glob.get_font("font/beroga.ttf", 30), os.str() );
} //floating_score_component::floating_score_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do an iteration.
 */
void rp::floating_score_component::progress
( bear::universe::time_type elapsed_time )
{
  m_tweeners.update(elapsed_time); 
} // floating_score_component::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the players oxygen.
 * \param e (out) The scene elements.
 */
void rp::floating_score_component::render
( status_component::scene_element_list& e ) const
{
  bear::visual::scene_sprite sp
    ( get_position().x - m_sprite.width()/2, 
      get_position().y - m_sprite.height()/2, 
      m_sprite );
  e.push_back( sp );
  
  bear::visual::scene_writing s1
    ( get_position().x - m_points.get_width() + 10, 
      get_position().y - 5, m_points );

  s1.get_rendering_attributes().set_intensity(0.0, 0.0, 0.0);
  e.push_back( s1 );
  
  bear::visual::scene_writing s2
    ( get_position().x + 30, 
      get_position().y -15, m_combo );

  s2.get_rendering_attributes().set_intensity(0.0, 0.0, 0.0);
  e.push_back( s2 );
} // floating_score_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the movement is finished.
 */
bool rp::floating_score_component::is_finished() const
{
  return m_tweeners.is_finished();
} // floating_score_component::is_finished()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the current position of the component.
 */
const bear::visual::position_type& 
rp::floating_score_component::get_position() const
{
  return m_position;
} // floating_score_component::get_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set position.
 * \param init_position The init position.
 * \param end_position The end position.
 */
void rp::floating_score_component::set_position
( const bear::visual::position_type& init_position,
  const bear::visual::position_type& end_position)
{
  m_position = init_position;

  const claw::tween::single_tweener tween_x
    ( init_position.x, end_position.x, 2.0,
      boost::bind
      (&floating_score_component::on_x_position_update,
       this, _1 ), &claw::tween::easing_quad::ease_in );
  
       m_tweeners.insert(tween_x);

  const claw::tween::single_tweener tween_y
    ( init_position.y, end_position.y, 2.0,
      boost::bind
      (&floating_score_component::on_y_position_update,
       this, _1 ), &claw::tween::easing_quad::ease_in );
  
  m_tweeners.insert(tween_y);
} // floating_score_component::set_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the combo value.
 */
unsigned int rp::floating_score_component::get_combo_value() const
{  
  return m_combo_value;
} // floating_score_component::get_combo_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the points value.
 */
int rp::floating_score_component::get_points_value() const
{  
  return m_points_value;
} // floating_score_component::get_points_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the x-position of the component.
 * \param x The x-position of the component.
 */
void rp::floating_score_component::on_x_position_update(double x)
{  
  m_position.x = x;
} // floating_score_component::on_x_position_update()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the y-position of the component.
 * \param y The y-position of the component.
 */
void rp::floating_score_component::on_y_position_update(double y)
{
  m_position.y = y;
} // floating_score_component::on_y_position_update()
