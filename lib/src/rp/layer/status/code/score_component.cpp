/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::score_component class.
 * \author Sebastien Angibaud
 */
#include "rp/layer/status/score_component.hpp"

#include "rp/layer/status/floating_score_component.hpp"

#include "rp/game_variables.hpp"

#include "engine/game.hpp"

#include "visual/bitmap_writing.hpp"
#include "visual/scene_writing.hpp"
#include "visual/scene_sprite.hpp"

#include <boost/bind.hpp>
#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_elastic.hpp>
#include <claw/tween/easing/easing_linear.hpp>

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
rp::score_component::score_component
( bear::engine::level_globals& glob,
  const bear::visual::position_type& active_position,
  bear::universe::zone::position side,
  x_placement x_p, y_placement y_p,
  const bear::universe::size_box_type& layer_size,
  const bear::universe::coordinate_type& hide_height, bool flip )
  : super(glob,active_position,side, x_p, y_p, layer_size, hide_height, flip),
    m_font(glob.get_font("font/LuckiestGuy.ttf", 30))
{

} // score_component::score_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the component.
 */
void rp::score_component::build()
{
  std::ostringstream oss;
  oss << game_variables::get_score();
  m_score.create(m_font, oss.str());
  m_score->set_intensity(0, 0, 0);

  super::build();
} // score_component::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do an iteration.
 */
void rp::score_component::progress
( bear::universe::time_type elapsed_time )
{
  super::progress(elapsed_time );

  floating_score_list::iterator it =
    m_floating_score.begin();

  while ( it != m_floating_score.end() )
    {
      it->progress(elapsed_time);

      if ( it->is_finished() )
	{
	  on_score_added
            (it->get_combo_value(), it->get_points_value());

          it = m_floating_score.erase(it);
	}
      else
        ++it;
    }

  std::ostringstream oss;
  oss << game_variables::get_score();
  m_score.create(m_font, oss.str());
  m_score->set_intensity(0, 0, 0);
} // score_component::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the component.
 * \param e (out) The scene elements.
 */
void rp::score_component::render( scene_element_list& e ) const
{
  if ( ! game_variables::is_level_ending() )
    {
      bear::visual::scene_writing s
        ( get_render_position().x + ( width() - m_score.get_width() ) / 2,
          get_render_position().y + ( height() - m_score.get_height() ) / 2,
          m_score );

      e.push_back( s );
    }

  floating_score_list::const_iterator it;
  for ( it = m_floating_score.begin();
	it != m_floating_score.end(); ++it )
    it->render(e);
} // score_component::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual width of the bar.
 */
unsigned int rp::score_component::width() const
{
  return 111;
} // score_component::width()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the visual height of the bar.
 */
unsigned int rp::score_component::height() const
{
  return 29;
} // score_component::height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::score_component::init_signals()
{
  super::init_signals();

  add_signal
    ( bear::engine::game::get_instance().listen_bool_variable_change
      ( game_variables::get_new_score_variable_name(),
        boost::bind
        (&rp::score_component::on_new_score,
         this, _1) ) );
} // score_component::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when score changes.
 * \param combo The combo value.
 * \param points The points value.
 */
void rp::score_component::on_score_added(unsigned int combo, int points)
{
  int c = combo * points;
  if ( (c < 0) && ((int)game_variables::get_score() < -c) )
    game_variables::set_score( 0 );
  else
    game_variables::set_score( game_variables::get_score() + c );

  update_inactive_position();
}  // score_component::on_score_changed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create tweener of movement.
 */
void rp::score_component::create_tweener()
{
  claw::tween::tweener_sequence tween;

  tween.insert
    ( claw::tween::single_tweener
      (get_position().x, get_active_position().x + 30, 0.5,
       boost::bind
       ( &rp::status_component::on_x_position_update,
         this, _1 ), &claw::tween::easing_elastic::ease_out ) );

  tween.insert
    ( claw::tween::single_tweener
      (get_active_position().x + 30, get_active_position().x, 0.3,
       boost::bind
       ( &rp::status_component::on_x_position_update,
	 this, _1 ), &claw::tween::easing_linear::ease_out ) );

  add_tweener( tween );
} // score_component::create_tweener()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when new_score game variable changes.
 * \param value The value of the variable new_score.
 */
void rp::score_component::on_new_score(bool value)
{
  if ( value )
    {
      bear::visual::position_type pos(get_layer_size()/2);
      pos.x +=
        ( game_variables::get_score_rate_x() * get_layer_size().x);
      pos.y +=
        ( game_variables::get_score_rate_y() * get_layer_size().y);

      bear::visual::position_type end_pos(pos);
      end_pos.y = get_layer_size().y;

      floating_score_component f(get_level_globals());

      m_floating_score.push_back(f);
      m_floating_score.back().set_position
        (pos, get_render_position() +
         bear::universe::position_type( width() / 2, 0 ) );

      game_variables::set_new_score(false);
    }
} // score_component::on_new_score()
