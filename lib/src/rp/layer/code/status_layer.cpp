/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::status_layer class.
 * \author Julien Jorge
 */
#include "rp/layer/status_layer.hpp"

#include "rp/cart.hpp"
#include "rp/layer/status/background_component.hpp"
#include "rp/layer/status/bonus_component.hpp"
#include "rp/layer/status/boss_component.hpp"
#include "rp/layer/status/balloon_component.hpp"
#include "rp/layer/status/cannonball_component.hpp"
#include "rp/layer/status/lives_component.hpp"
#include "rp/layer/status/plunger_component.hpp"
#include "rp/layer/status/score_component.hpp"
#include "rp/layer/status/status_component.hpp"

#include "rp/game_variables.hpp"
#include "engine/game.hpp"
#include "engine/level.hpp"
#include "visual/bitmap_writing.hpp"
#include "visual/scene_sprite.hpp"
#include "visual/scene_writing.hpp"
#include <boost/bind.hpp>

/*----------------------------------------------------------------------------*/
const unsigned int rp::status_layer::s_margin = 5;

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::status_layer::~status_layer()
{
  component_list::iterator it2;
  for ( it2 = m_components.begin(); 
	it2 != m_components.end(); ++it2)
    delete *it2;
} // status_layer::~status_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the layer.
 */
void rp::status_layer::build()
{
  super::build();
  
  m_score_background =
    get_level_globals().auto_sprite
    ( "gfx/status/status.png", "score background" );
  create_components();
} // status_layer::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the players status and update local values.
 */
void rp::status_layer::progress( bear::universe::time_type elapsed_time )
{
  if ( ! get_level().is_paused() )
    {
      component_list::iterator it2;
      for ( it2 = m_components.begin(); 
            it2 != m_components.end(); ++it2)
        (*it2)->progress(elapsed_time);
    }
} // status_layer::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the players status.
 * \param e (out) The scene elements.
 */
void rp::status_layer::render( scene_element_list& e ) const
{
  if ( game_variables::level_has_started() &&
       ! game_variables::is_level_ending() && 
       ! get_level().is_paused() && 
       ! game_variables::is_boss_transition() )
    {
      component_list::const_iterator it;
      for ( it = m_components.begin(); 
            it != m_components.end(); ++it)
        (*it)->render( e );
    }
} // status_layer::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create  components.
 */
void rp::status_layer::create_components()
{
  if ( game_variables::is_boss_level() )
    return;

#ifdef __ANDROID__
  create_components_top_left();
#else
  create_components_bottom_right();
#endif
} // status_layer::create_components()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the components in the top left corner.
 */
void rp::status_layer::create_components_top_left()
{
  const bear::visual::position_type pos_top( 0, get_size().y );

  create_status_component<background_component>
    ( pos_top, bear::universe::zone::middle_left_zone );

  create_status_component<balloon_component>
    ( pos_top + bear::universe::position_type( 7, -35 ),
      bear::universe::zone::middle_left_zone );

  create_status_component<score_component>
    ( pos_top + bear::universe::position_type( 7, -77 ),
      bear::universe::zone::middle_left_zone );
} // status_layer::create_components_top_left()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the components in the bottom right corner.
 */
void rp::status_layer::create_components_bottom_right()
{
  const bear::visual::position_type pos_top
    ( get_size().x - m_score_background.width(), m_score_background.height() );

  create_status_component<background_component>
    ( pos_top, bear::universe::zone::middle_right_zone );

  create_status_component<balloon_component>
    ( pos_top + bear::universe::position_type( 60, -35 ),
      bear::universe::zone::middle_right_zone );

  create_status_component<score_component>
    ( pos_top + bear::universe::position_type( 60, -77 ),
      bear::universe::zone::middle_right_zone );
} // status_layer::create_components_bottom_right()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates and add a new status component in the layer.
 * \param T the type of the status component.
 * \param active_position The position of the component in the layer when it is
 *        visible.
 * \param size The size on which the component appears.
 */
template<typename T>
T* rp::status_layer::create_status_component
( const bear::visual::position_type& active_position,
  bear::universe::zone::position side )
{
  const bool flip( side == bear::universe::zone::middle_right_zone );
  const double hide_direction( flip ? 1 : -1 );
  typename status_component::x_placement x_p
    ( flip ? status_component::right_placement
      : status_component::left_placement );

  T* const result
    ( new T
      ( get_level_globals(), active_position, side,
        x_p, status_component::top_placement,
        get_size(), 0.8 * hide_direction * m_score_background.width(), flip ) );

  result->build();
  m_components.push_back( result );

  return result;
} // status_layer::create_status_component()
