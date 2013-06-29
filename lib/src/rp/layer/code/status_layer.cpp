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
rp::status_layer::status_layer()
  : m_timer(NULL)
{
} // status_layer::status_layer()

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
 * \brief Set the timer to use for the time.
 */
void rp::status_layer::set_level_timer( const time_component::timer_handle& t )
{
  m_timer = t; 
} // status_layer::set_level_timer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create  components.
 */
void rp::status_layer::create_components()
{

#if 0
  // time
  if ( m_timer != time_component::timer_handle(NULL) && 
       ! game_variables::is_boss_level() )
    {
      bear::visual::position_type pos_time( s_margin, s_margin);
      status_component* time = new time_component
        (get_level_globals(), pos_time, bear::universe::zone::bottom_zone, 
         status_component::left_placement,
         status_component::bottom_placement, get_size(), 0, false, m_timer); 
      time->build();
      m_components.push_back(time); 
    }
#endif
  
#if 0
  // ############## TOP LEFT #########################
  bear::visual::position_type pos_top( 0, get_size().y);
  if ( game_variables::is_boss_level() )
    pos_bottom.y += 34 + s_margin; // + score_size - s_margin

  // background
  background_component * ba = new background_component
    (get_level_globals(), pos_top, bear::universe::zone::top_zone, 
     status_component::left_placement,
     status_component::top_placement,get_size(),
     - m_score_background.get_size().y,true); 
  ba->build();
  m_components.push_back(ba);  

  if ( game_variables::is_boss_level() )
    pos_bottom.y -= 34 + s_margin; // - score_size - s_margin

  // score
  if ( ! game_variables::is_boss_level() )
    {
      pos_top.y = get_size().y - s_margin;
      score_component* c = new score_component
        (get_level_globals(), pos_top, bear::universe::zone::top_zone, 
         status_component::left_placement,
         status_component::top_placement,get_size(),
         - m_score_background.get_size().y, true); 
      c->build();
      m_components.push_back(c);
      pos_top.y -= c->height() + s_margin;
    }

  // balloon
  status_component* b;
  if ( game_variables::is_boss_level() )
    b = new boss_component
      (get_level_globals(), pos_top, bear::universe::zone::top_zone, 
       status_component::left_placement,
       status_component::top_placement,get_size(),
       - m_score_background.get_size().y, true); 
  else
     b = new balloon_component
    (get_level_globals(), pos_top, bear::universe::zone::top_zone, 
     status_component::left_placement,
     status_component::top_placement,get_size(),
     - m_score_background.get_size().y, true); 
  b->build();
  m_components.push_back(b);

  // plunger bonus
  status_component* bonus = new bonus_component
    ( get_level_globals(),
      bear::visual::position_type( pos_top.x + 83, pos_top.y - 101),
      bear::universe::zone::top_zone, 
      status_component::left_placement,
      status_component::bottom_placement, get_size(),
      - m_score_background.get_size().y, true); 
  bonus->build();
  m_components.push_back(bonus); 

  // lives
  pos_top.y -= b->height() + s_margin;
  status_component* l = new lives_component
    (get_level_globals(), pos_top, bear::universe::zone::top_zone, 
     status_component::left_placement,
     status_component::top_placement,get_size(),
     - m_score_background.get_size().y, true); 
  l->build();
  m_components.push_back(l);
#endif

  // ############## BOTTOM RIGHT #########################
  bear::visual::position_type pos_bottom
    ( get_size().x - m_score_background.get_size().x, 0 );

  if ( game_variables::is_boss_level() )
    pos_bottom.y -= 34 + s_margin; // + score_size + s_margin

  // background
  status_component* const ba = new background_component
    (get_level_globals(), pos_bottom, bear::universe::zone::bottom_zone, 
     status_component::left_placement,
     status_component::bottom_placement,get_size(),
     m_score_background.get_size().y,false); 
  ba->build();
  m_components.push_back(ba);  

  if ( game_variables::is_boss_level() )
    pos_bottom.y += 34 + s_margin; // + score_size + s_margin

  // score
  pos_bottom.x += s_margin;
  pos_bottom.y += s_margin;
  
  if ( ! game_variables::is_boss_level() )
    {
      score_component* c = new score_component
        (get_level_globals(), pos_bottom, bear::universe::zone::bottom_zone, 
         status_component::left_placement,
         status_component::bottom_placement,get_size(),
         m_score_background.get_size().y, false); 
      c->build();
      m_components.push_back(c);
      pos_bottom.y += c->height() + s_margin;
    }

  // balloon
  status_component* b;
  if ( game_variables::is_boss_level() )
    b = new boss_component
      (get_level_globals(), pos_bottom, bear::universe::zone::bottom_zone, 
       status_component::left_placement,
       status_component::bottom_placement,get_size(),
       m_score_background.get_size().y, false); 
  else
     b = new balloon_component
    (get_level_globals(), pos_bottom, bear::universe::zone::bottom_zone, 
     status_component::left_placement,
     status_component::bottom_placement,get_size(),
     m_score_background.get_size().y, false); 
  b->build();
  m_components.push_back(b);

  // plunger bonus
  pos_bottom.y += b->height() + s_margin;
  status_component* bonus = new bonus_component
    ( get_level_globals(),
      bear::visual::position_type
      ( pos_bottom.x + 30, pos_bottom.y + s_margin),
      bear::universe::zone::bottom_zone, 
      status_component::left_placement,
      status_component::bottom_placement, get_size(),
      m_score_background.get_size().y, false); 
  bonus->build();
  m_components.push_back(bonus); 

  // lives
  pos_bottom.x = get_size().x - s_margin;
  status_component* l = new lives_component
    (get_level_globals(), pos_bottom, bear::universe::zone::bottom_zone, 
     status_component::right_placement,
     status_component::bottom_placement,get_size(),
     m_score_background.get_size().y, false); 
  l->build();
  m_components.push_back(l);

#if 0
  // plunger
  pos_bottom.x = 
    get_size().x / 2 - 185 + /* a quarter of the cannon's width */ 15;
  pos_bottom.y = 0;
  status_component* p = new plunger_component
    (get_level_globals(), pos_bottom, bear::universe::zone::bottom_zone, 
     status_component::left_placement,
     status_component::bottom_placement, get_size(), 74, false); 
  p->build();
  m_components.push_back(p);

  pos_top.x = pos_bottom.x;
  pos_top.y = get_size().y;
  p = new plunger_component
    (get_level_globals(), pos_top, bear::universe::zone::top_zone, 
     status_component::left_placement,
     status_component::top_placement, get_size(), -74, true); 
  p->build();
  m_components.push_back(p);

  // cannonball
  pos_bottom.x += p->width() - /* half of the cannon's width */ 30;
  status_component* ca = new cannonball_component
    (get_level_globals(), pos_bottom, bear::universe::zone::bottom_zone, 
     status_component::left_placement,
     status_component::bottom_placement, get_size(), 74, false); 
  ca->build();
  m_components.push_back(ca); 
 
  pos_top.x = pos_bottom.x;
  ca = new cannonball_component
    (get_level_globals(), pos_top, bear::universe::zone::top_zone, 
     status_component::left_placement,
     status_component::top_placement,get_size(), -74, true); 
  ca->build();
  m_components.push_back(ca); 
#endif
} // status_layer::create_components()
