/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the level_selector class.
 * \author Sebastien Angibaud
 */
#include "rp/level_selector.hpp"
#include "rp/defines.hpp"
#include "rp/game_variables.hpp"
#include "rp/util.hpp"

#include "rp/ad/show_interstitial.hpp"
#include "rp/events/tag_event.hpp"
#include "rp/events/make_event_property.hpp"

#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "engine/scene_visual.hpp"
#include "engine/game.hpp"
#include "engine/variable/variable.hpp"
#include "engine/variable/variable_list_reader.hpp"
#include "engine/variable/var_map.hpp"

#include "generic_items/star.hpp"

#include "visual/scene_sprite.hpp"
#include "visual/scene_writing.hpp"

#include "universe/forced_movement/forced_tracking.hpp"

#include <boost/bind.hpp>
#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/single_tweener.hpp>
#include <claw/tween/easing/easing_linear.hpp>
#include <claw/tween/easing/easing_sine.hpp>
#include <claw/tween/easing/easing_back.hpp>
#include <claw/tween/easing/easing_elastic.hpp>

#include "sstream"

BASE_ITEM_EXPORT( level_selector, rp )

namespace rp
{
  namespace detail
  {
    static constexpr unsigned int state_locked = 0;
    static constexpr unsigned int state_unlocked = 1;
    static constexpr unsigned int state_completed = 2;
    static constexpr unsigned int state_bronze = 3;
    static constexpr unsigned int state_silver = 4;
    static constexpr unsigned int state_gold = 5;
  }
}

bool rp::level_selector::s_selection = false;

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::level_selector::level_selector()
: m_updated(false), m_required_medal(2), m_bronze_threshold(0),  
  m_silver_threshold(0), m_gold_threshold(0), m_required_balloons(0),
  m_init_level_factor(0.25), m_visible(true), 
  m_theme("western"), m_level_name_gap_y(0.0),
  m_mouse_in(false), m_rectangle(NULL),
  m_decorative_level_name(NULL), m_load(false), m_rectangle_opacity(0),
  m_medal_movement(1), m_cursor(NULL)
{
  set_artificial( true );
  set_phantom( true );
  set_can_move_items( false );
} // rp::level_selector::level_selector()

rp::level_selector::level_selector( const level_selector& that )
{
  assert( false );
}

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::level_selector::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_image("gfx/status/level/frame-1.png");
  get_level_globals().load_image("gfx/status/level/frame-2.png");
  get_level_globals().load_image("gfx/status/level/frame-3.png");
  get_level_globals().load_image("gfx/status/level/frame-4.png");
  get_level_globals().load_image("gfx/status/level/frame-theme-1.png");
  get_level_globals().load_image("gfx/status/level/frame-theme-2.png");
  get_level_globals().load_image("gfx/status/medal/medal.png");

  get_level_globals().load_sound("sound/tick.ogg");
} // level_selector::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::level_selector::on_enters_layer()
{
  super::on_enters_layer();

  m_level_state = 
    game_variables::get_level_state(m_serial_number, m_level_number);
  m_font = get_level_globals().get_font("font/LuckiestGuy.ttf", 70);

  choose_level_sprite();

  if ( is_boss_level() )
    {
      m_background_sprite =
        get_level_globals().auto_sprite
        ( "gfx/status/level/frame-2.png", "main" );
      m_hidden_sprite = m_background_sprite;
      m_border_sprite =
        get_level_globals().auto_sprite
        ( "gfx/status/level/frame-4.png", "bright frame" );
      m_background_gap = bear::visual::position_type(-13,-13);
      m_init_level_factor *= 2;
    }
  else
    {
      m_background_sprite =
        get_level_globals().auto_sprite
        ( "gfx/status/level/frame-1.png", "main" );
      m_hidden_sprite =
        get_level_globals().auto_sprite
        ( "gfx/status/level/frame-2.png", "empty" );

      m_hidden_sprite.set_size( m_background_sprite.get_size() );

      m_border_sprite =
        get_level_globals().auto_sprite
        ( "gfx/status/level/frame-3.png", "bright frame" );
      m_background_gap = bear::visual::position_type(-108,-167);
      m_medal_gap = bear::visual::position_type(-133,-162);
    }

  m_star_gap = bear::visual::position_type(131,133);
  m_level_factor = m_init_level_factor;
  m_medal_factor = 0.93478;
  m_score_factor = 1; //0.8;
  set_size( m_level_sprite.get_size() * m_level_factor );

  m_star =
    bear::visual::animation
    ( get_level_globals().auto_sprite
      ( "gfx/status/level/frame-2.png", "star" ) );

  game_variables::select_level( false );
  util::load_game_variables(); 
  update_medal(get_state());
  update_state();  
  m_initial_position = get_center_of_mass();

  m_tweener_rectangle_opacity = 
    claw::tween::single_tweener
    ( m_rectangle_opacity, 0.0, 1.0, 
      &claw::tween::easing_linear::ease_in_out );

  m_medal_movement = 1.0;
  m_tweener_medal =
    claw::tween::single_tweener
    ( m_medal_movement, 1.0, 0.5, &claw::tween::easing_linear::ease_in );

  m_tweener_star = 
    claw::tween::single_tweener
    ( 0.0, 0.0, 1.0,
      boost::bind
      ( &rp::level_selector::on_star_angle_change,
        this, _1 ), &claw::tween::easing_linear::ease_out );

  create_interactive_item(*this, 1, 0);
} // level_selector::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_selector::progress( bear::universe::time_type elapsed_time )
{
  if ( m_ad_connection.connected() )
    return;
  
  super::progress( elapsed_time );

  game_variables::set_boss_transition(false);
  progress_input_reader(elapsed_time);

  m_star.next(elapsed_time);
  m_tweeners.update(elapsed_time);
  m_tweener_rectangle_opacity.update(elapsed_time);
  m_tweener_medal.update(elapsed_time);
  m_tweener_star.update(elapsed_time);
  
  if ( m_rectangle != NULL && m_rectangle_opacity > 0 )
    m_rectangle->get_rendering_attributes().set_opacity(m_rectangle_opacity);
  
  if ( m_decorative_level_name != NULL )
    {
      m_decorative_level_name->set_center_of_mass
      ( util::get_level_name_position(get_level().get_camera_focus()) );
      m_decorative_level_name->set_gap_y( m_level_name_gap_y );
    }

  if ( ! m_updated && ( ! s_selection || is_selected_level() ) )
    update_state();

  if ( game_variables::get_serial_number() == m_serial_number && 
       game_variables::get_level_number() == m_level_number )
    set_z_position( 100 );
  else
    set_z_position( 0 );

  if ( is_selected_level() )
    check_orders();
  
  set_passive
    ( ( m_level_state == detail::state_locked )
      || ( s_selection && ! is_selected_level() ) );
} // level_selector::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type item_list.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::level_selector::set_item_list_field
( const std::string& name, const std::vector<bear::engine::base_item*>& value )
{
  bool ok = true;

  if (name == "level_selector.precedence")
    for( std::size_t i=0; i!=value.size(); ++i )
      {
        level_selector* l = dynamic_cast<level_selector*>(value[i]);
        m_precedence.push_back( handle_type(l) );
      }
  else
    ok = super::set_item_list_field(name, value);

  return ok;
} // level_selector::set_item_list_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type item.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::level_selector::set_item_field
( const std::string& name, bear::engine::base_item* value )
{
  bool ok = true;

  if ( name == "level_selector.cursor")
    {
      bear::cursor* a_cursor = dynamic_cast<bear::cursor*>(value);
      if ( a_cursor != NULL )
        m_cursor = a_cursor;
    }
  else
    ok = super::set_item_field(name, value);

  return ok;
} // level_selector::set_item_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c unsigned int.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool
rp::level_selector::set_u_integer_field
( const std::string& name, unsigned int value )
{
  bool result = true;

  if (name == "level_selector.level_number")
    m_level_number = value;
  else if (name == "level_selector.serial_number")
    m_serial_number = value;
  else if (name == "level_selector.required_medal")
    m_required_medal = value + 1;
  else if (name == "level_selector.threshold.bronze")
    m_bronze_threshold = value;
  else if (name == "level_selector.threshold.silver")
    m_silver_threshold = value;
  else if (name == "level_selector.threshold.gold")
    m_gold_threshold = value; 
  else if (name == "level_selector.required_balloons")
    m_required_balloons = value;
  else
    result = super::set_u_integer_field(name, value);

  return result;
} // level_selector::set_u_integer_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c string int.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool
rp::level_selector::set_string_field
( const std::string& name, const std::string& value )
{
  bool result = true;

  if (name == "level_selector.theme")
    m_theme = value;
  else if (name == "level_selector.level_name")
    m_level_name = gettext(value.c_str());
  else
    result = super::set_string_field(name, value);

  return result;
} // level_selector::set_string_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the sprite representing the item.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::get_visual
( std::list<bear::engine::scene_visual>& visuals ) const
{
  if ( m_visible )
    {
      render_background(visuals);
      render_level(visuals);
      render_border(visuals);
      render_star(visuals);
      
      if ( ! is_boss_level() )
        {
          render_medal(visuals);
          render_points(visuals);
        }
    }
  else
    render_hidden_sprite(visuals);
} // level_selector::get_visuals();

/*----------------------------------------------------------------------------*/
/**
 * \brief Executes the action associated with the selector.
 */
void rp::level_selector::activate()
{
  if ( m_level_state > detail::state_locked )
    {
      if ( ! s_selection && 
           std::abs(m_level_factor - m_init_level_factor) <= 0.1 &&
           ! game_variables::get_movement_order_status() )
        select_level();
      else if ( is_selected_level() )
        {
          if ( m_level_factor > 0.99 )
            game_variables::set_go_order_status(true);
          check_go_order();
        }
    }
  else
    tag_event
      ( "level-selected",
        {
          make_event_property( "locked", "true" ),
          make_event_property( "serial", m_serial_number ),
          make_event_property( "level", m_level_number ),
          make_event_property( "completed", false )
        } );
} // level_selector::activate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Processes a finger event. If the finger is released on the button, the
 *        button is activated.
 * \param event The event to process
 */
bool rp::level_selector::finger_action
( const bear::input::finger_event& event )
{
  bool result = false;

  if ( ( event.get_type() == bear::input::finger_event::finger_event_released )
       && ( get_bounding_box().includes
            ( get_level().screen_to_level( event.get_position() ) ) ) )
    {
      result = true;
      activate();
    }

  return result;
} // level_selector::finger_action()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the player to stop the action associated with a mouse button.
 * \param button The code of the button.
 * \param pos The position of the cursor on the screen.
 */
bool rp::level_selector::mouse_released
( bear::input::mouse::mouse_code button,
  const claw::math::coordinate_2d<unsigned int>& pos )
{
  bool result = false;

#ifndef NDEBUG
  if ( button == bear::input::mouse::mc_right_button )
    {
      if ( get_bounding_box().includes( get_level().screen_to_level(pos) ) )
        {
          result = true;
          if ( ! s_selection && 
                 std::abs(m_level_factor - m_init_level_factor) <= 0.1 &&
                 ! game_variables::get_movement_order_status() )
            select_level();
        }
    }
  else
#endif
    if ( button == bear::input::mouse::mc_left_button )
    {
      if ( get_bounding_box().includes( get_level().screen_to_level(pos) ) )
        {
          result = true;
          activate();
        }
    }

  return result;
} // level_selector::mouse_released()

/*----------------------------------------------------------------------------*/
/**
 * \brief Method called when the position of the mouse changes.
 * \param pos The new position of the cursor.
 * \return true if the event has been processed.
 */
bool rp::level_selector::mouse_move
( const claw::math::coordinate_2d<unsigned int>& pos )
{
  bool in = false;

  if ( ! s_selection && m_visible && m_level_state > detail::state_locked )
    in = get_bounding_box().includes( get_level().screen_to_level(pos) );

  if ( in && ! m_mouse_in )
    get_level_globals().play_sound
      ( "sound/tick.ogg", 
        bear::audio::sound_effect( get_center_of_mass() ) );

  m_mouse_in = in;

  return false;
} // level_selector::mouse_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Method called when the position of the mouse changes.
 */
void rp::level_selector::load_level()
{
  tag_event
    ( "load-level",
      {
        make_event_property( "serial", m_serial_number ),
        make_event_property( "level", m_level_number  )
      } );
  
  m_load = true;
  const double fade_duration(1);

  if ( m_rectangle != NULL )
    { 
      m_rectangle->get_rendering_attributes().set_opacity(1.0);

      m_tweener_rectangle_opacity = 
        claw::tween::single_tweener
        ( m_rectangle_opacity, 1.0, fade_duration, 
          &claw::tween::easing_linear::ease_in_out );
    }

  get_level().stop_music( fade_duration );

  claw::tween::tweener_sequence t;
  t.insert
    ( claw::tween::single_tweener
      ( get_vertical_middle(), 
        get_vertical_middle() - get_level().get_camera_focus().size().y, 
        fade_duration / 2,
       boost::bind
       ( &rp::level_selector::on_gap_y_change,
         this, _1 ), &claw::tween::easing_back::ease_in ) );
  t.insert
    ( claw::tween::single_tweener
      ( get_vertical_middle() - get_level().get_camera_focus().size().y, 
        get_vertical_middle() - get_level().get_camera_focus().size().y, 
        fade_duration / 2,
        boost::bind
        ( &rp::level_selector::on_gap_y_change,
          this, _1 ), &claw::tween::easing_back::ease_in ) );
  t.on_finished
    (boost::bind( &rp::level_selector::push_level, this ));

  m_tweeners.insert(t);
} // level_selector::load_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update visibility of the selector.
 */
void rp::level_selector::update_visibility()
{
  if ( m_serial_number == 6 )
    m_visible = game_variables::get_level_state( m_serial_number, 1 ) > 0;
  else if ( m_level_number >= 5 &&  m_level_number <= 7 )
    m_visible = game_variables::get_level_state( m_serial_number, 5 ) > 0;  
} // level_selector::update_visibility()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update state of the selector.
 */
void rp::level_selector::update_state()
{
  unsigned int state = m_level_state;
  unsigned int new_state = get_new_state();
  
  if ( state == detail::state_locked )
    m_level_sprite.set_opacity(0);
    
  if ( state <= detail::state_unlocked ) 
    m_star.set_opacity(0);

  if ( state <= detail::state_completed )
    m_medal_sprite.set_intensity(0,0,0);
  
  if ( ( state == detail::state_locked )
       && ( new_state >= detail::state_unlocked ) )
    unlock();
  
  if ( ( state <= detail::state_unlocked )
       && ( new_state >= detail::state_completed ) )
    show_star();
  
  if ( ( new_state >= detail::state_bronze ) && ( state < new_state ) )
    show_medal(new_state);
    
  update_score();
  update_visibility();
} // level_selector::update_state()

/*----------------------------------------------------------------------------*/
  /**
   * \brief Update score of the level.
   */
void rp::level_selector::update_score()
{
  std::ostringstream oss;
  unsigned int score = 
    game_variables::get_persistent_score(m_serial_number,m_level_number);
  
  if ( score > 0 ) 
    oss << score;
  
  m_points.create(m_font, oss.str() );
} // level_selector::update_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Check if previous level requires conditions.
 */
bool rp::level_selector::check_precedence() const
{
  bool result = true;
  
  handle_list::const_iterator it;

  for ( it = m_precedence.begin(); (it != m_precedence.end()) && result; ++it )
    result =
      ( (*it)->get_state() >= m_required_medal )
      || ( ( (*it)->m_level_number == 8 )
           && ( (*it)->m_level_state >= detail::state_completed ) );

  return result;
} // level_selector::check_precedence();

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the state of the level.
 */
unsigned int rp::level_selector::get_state() const
{
  return game_variables::get_level_state(m_serial_number, m_level_number);
} // level_selector::get_state()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the new state of the level.
 */
unsigned int rp::level_selector::get_new_state() const
{
  bear::engine::variable<unsigned int> v
    (game_variables::get_persistent_score_variable_name
     (m_serial_number,m_level_number));
  
  if ( ! bear::engine::game::get_instance().game_variable_exists(v) )
    {
      if ( check_precedence() )
        return detail::state_unlocked;
      else
        return detail::state_locked;
    }
  else
    {
      unsigned int score = 
        game_variables::get_persistent_score(m_serial_number,m_level_number);
      
      if ( score >= m_gold_threshold )
        return detail::state_gold;
      else if ( score >= m_silver_threshold )
        return detail::state_silver;
      else if ( score >= m_bronze_threshold )
        return detail::state_bronze;
      else
        return detail::state_completed;
    }
} // level_selector::get_new_state();

/*----------------------------------------------------------------------------*/
/**
 * \brief Add background sprites.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::render_background
( std::list<bear::engine::scene_visual>& visuals ) const
{
  bear::visual::position_type pos
    ( get_left() + m_background_gap.x * m_level_factor, 
      get_bottom() + m_background_gap.y * m_level_factor);

  bear::visual::scene_sprite s( pos.x, pos.y, m_background_sprite);
  s.set_scale_factor(m_level_factor, m_level_factor);
  
  visuals.push_back( s );
} // level_selector::render_background()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add hidden sprites.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::render_hidden_sprite
( std::list<bear::engine::scene_visual>& visuals ) const
{
  bear::visual::position_type pos
    ( get_left() + m_background_gap.x * m_level_factor, 
      get_bottom() + m_background_gap.y * m_level_factor);

  bear::visual::scene_sprite s( pos.x, pos.y, m_hidden_sprite);
  s.set_scale_factor(m_level_factor, m_level_factor);
  
  visuals.push_back( s );
} // level_selector::render_hidden_sprite()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add border sprite.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::render_border
( std::list<bear::engine::scene_visual>& visuals ) const
{
  bear::visual::position_type pos
    ( get_left() + m_background_gap.x * m_level_factor, 
      get_bottom() + m_background_gap.y * m_level_factor);

  bear::visual::scene_sprite s( pos.x, pos.y, m_border_sprite);
  
  if ( ( s_selection && ! is_selected_level() ) || 
       ( ! s_selection && ! m_mouse_in ) || 
       ( ! s_selection && 
         std::abs(m_level_factor - m_init_level_factor) > 0.1 ) ) 
    s.get_rendering_attributes().set_opacity(0);
  
  s.set_scale_factor(m_level_factor, m_level_factor);
  
  visuals.push_back( s );
} // level_selector::render_border()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the level sprite.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::render_level
( std::list<bear::engine::scene_visual>& visuals ) const
{
  bear::visual::scene_sprite s
    ( get_left(), get_bottom(), m_level_sprite);
  s.set_scale_factor(m_level_factor, m_level_factor);
  
  if ( ( s_selection && ! is_selected_level() ) || 
       ( ! s_selection && ! m_mouse_in ) ) 
    s.get_rendering_attributes().set_intensity(0.8,0.8,0.8);

  visuals.push_back( s );
} // level_selector::render_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the star sprite.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::render_star
( std::list<bear::engine::scene_visual>& visuals ) const
{
  bear::visual::position_type pos
    ( get_horizontal_middle() + m_level_factor * 
      ( m_star_gap.x - (double)m_star.get_sprite().width() / 2 ), 
      get_vertical_middle() + m_level_factor * 
      ( m_star_gap.y - (double)m_star.get_sprite().height() / 2 ));

  bear::visual::scene_sprite s( pos.x, pos.y, m_star.get_sprite());
  s.set_scale_factor(m_level_factor,m_level_factor);

  visuals.push_back( s );
} // level_selector::render_star()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the points.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::render_points
( std::list<bear::engine::scene_visual>& visuals ) const
{
  bear::visual::position_type pos
    ( get_right() + m_level_factor * m_score_factor *
      ( 50 - (double)m_points.get_width() ), 
      get_bottom() + m_level_factor * m_score_factor * 
      ( -60 - (double)m_points.get_height() ));
  
  bear::visual::scene_writing s( pos.x, pos.y, m_points );
  s.set_scale_factor
    (m_level_factor * m_score_factor, m_level_factor * m_score_factor); 

  visuals.push_back( s );
} // level_selector::render_points
  
/*----------------------------------------------------------------------------*/
/**
 * \brief Get the medal sprite.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_selector::render_medal
( std::list<bear::engine::scene_visual>& visuals ) const
{
  if ( !m_medal_sprite.is_valid() )
    return;
  
  double factor = m_level_factor * m_medal_factor;
  bear::visual::position_type pos = get_medal_position();

  pos = pos - factor * m_medal_sprite.get_size() / 2;

  bear::visual::scene_sprite s( pos.x, pos.y, m_medal_sprite);
  s.set_scale_factor( factor, factor);

  visuals.push_back( s );
} // level_selector::render_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start update.
 */
void rp::level_selector::start_update()
{
  m_updated = true;
  set_z_position(get_z_position() + 10);
} // level_selector::start_update()

/*----------------------------------------------------------------------------*/
/**
 * \brief End of the update.
 */
void rp::level_selector::end_update()
{
  m_updated = false;
  set_z_position(get_z_position() - 10);  
} // level_selector::end_update()

/*----------------------------------------------------------------------------*/
/**
 * \brief Unlock the level.
 */
void rp::level_selector::unlock()
{
  tag_event
    ( "level-unlocked",
      {
        make_event_property( "serial", m_serial_number ),
        make_event_property( "level", m_level_number  )
      } );

  start_update();

  if ( game_variables::get_unlocked_serial() < m_serial_number )
    game_variables::set_unlocked_serial(m_serial_number);
      
  // rotation
  claw::tween::tweener_sequence tween1;
  tween1.insert
    ( claw::tween::single_tweener
      (0.0, 0.52, 0.25,
       boost::bind
       ( &rp::level_selector::on_unlock_angle_change,
         this, _1 ), &claw::tween::easing_linear::ease_out ) );
  tween1.insert
    ( claw::tween::single_tweener
      (0.52, -0.52, 0.5,
       boost::bind
       ( &rp::level_selector::on_unlock_angle_change,
         this, _1 ), &claw::tween::easing_linear::ease_out ) );
  tween1.insert
    ( claw::tween::single_tweener
      (-0.52, 0.0, 0.25,
       boost::bind
       ( &rp::level_selector::on_unlock_angle_change,
         this, _1 ), &claw::tween::easing_linear::ease_out ) );

  tween1.on_finished
    ( boost::bind( &rp::level_selector::on_unlock_change, this ));
  
  m_tweeners.insert(tween1);

  // zoom
  claw::tween::tweener_sequence tween2;
  tween2.insert
    ( claw::tween::single_tweener
      (m_level_factor, m_init_level_factor*2, 0.5,
       boost::bind
       ( &rp::level_selector::on_unlock_factor_change,
         this, _1 ), &claw::tween::easing_linear::ease_out ) );
  tween2.insert
    ( claw::tween::single_tweener
      (m_init_level_factor * 2, m_init_level_factor, 0.5,
       boost::bind
       ( &rp::level_selector::on_unlock_factor_change,
         this, _1 ), &claw::tween::easing_linear::ease_out ) );
  
  m_tweeners.insert(tween2);
} // level_selector::unlock()

/*----------------------------------------------------------------------------*/
/**
 * \brief Show the star.
 */
void rp::level_selector::show_star()
{
  tag_event
    ( "level-validated",
      {
        make_event_property( "serial", m_serial_number ),
        make_event_property( "level", m_level_number  )
      } );
  
  start_update();
  m_star.set_opacity(1);

  m_tweener_star = 
    claw::tween::single_tweener
    ( 0.0, 6.283, 1.0,
      boost::bind
      ( &rp::level_selector::on_star_angle_change,
        this, _1 ), &claw::tween::easing_linear::ease_out );
  m_tweener_star.on_finished
    ( boost::bind( &rp::level_selector::on_star_change, this ) );
} // level_selector::show_star()

/*----------------------------------------------------------------------------*/
/**
 * \brief Show the medal.
 * \param medal The medal.
 */
void rp::level_selector::show_medal( unsigned int medal )
{
  start_update();

  if ( medal == 3 )
    m_medal_sprite =
      get_level_globals().auto_sprite( "gfx/status/medal/medal.png", "bronze" );
  else if ( medal == 4 )
    m_medal_sprite =
      get_level_globals().auto_sprite( "gfx/status/medal/medal.png", "silver" );
  else if ( medal == 5 )
    m_medal_sprite =
        get_level_globals().auto_sprite( "gfx/status/medal/medal.png", "gold" );
  
  m_medal_movement = 0;
  m_tweener_medal =
    claw::tween::single_tweener
    ( m_medal_movement, 1, 0.5, &claw::tween::easing_back::ease_in );
  m_tweener_medal.on_finished
    (boost::bind( &rp::level_selector::start_move_back, this ));
  
  double factor = m_medal_factor;
  m_medal_factor = 1;
  claw::tween::single_tweener t_factor
    ( claw::tween::single_tweener
      ( m_medal_factor, factor, 
        0.5, &claw::tween::easing_back::ease_in ) );
  m_tweeners.insert(t_factor); 
} // level_selector::show_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief The level is unlocked.
 */
void rp::level_selector::on_unlock_change()
{
  game_variables::set_level_state
    (m_serial_number, m_level_number, get_new_state());
  m_level_state = 
    game_variables::get_level_state( m_serial_number, m_level_number );
  update_visibility();
  m_level_sprite.set_opacity(1);

  end_update();
} // level_selector::on_unlock_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief The star state changes.
 */
void rp::level_selector::on_star_change()
{
  game_variables::set_level_state
    (m_serial_number, m_level_number, get_new_state());
  m_level_state = 
    game_variables::get_level_state( m_serial_number, m_level_number );

  m_tweener_star = 
    claw::tween::single_tweener
    ( 0.0, 0.0, 1.0,
      boost::bind
      ( &rp::level_selector::on_star_angle_change,
        this, _1 ), &claw::tween::easing_linear::ease_out );

  end_update();
} // level_selector::on_star_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief The gap on y-coordinate change.
 * \param gap_y The new gap.
 */
void rp::level_selector::on_gap_y_change
( bear::universe::coordinate_type gap_y )
{
  set_vertical_middle(gap_y);
} // level_selector::on_gap_y_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Angle of level sprite changes.
 * \param angle Angle for level sprite.
 */
void rp::level_selector::on_unlock_angle_change( double angle )
{
  m_level_sprite.set_angle(angle);
} // level_selector::on_unlock_angle_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Factor of level sprite changes.
 * \param factor Factor for level sprite.
 */
void rp::level_selector::on_unlock_factor_change( double factor )
{
  m_level_factor = factor;

  bear::universe::position_type center = get_center_of_mass();
  set_size(m_level_sprite.get_size() * m_level_factor);
  set_center_of_mass( center );
} // level_selector::on_unlock_factor_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Push the level.
 */
void rp::level_selector::push_level()
{
  util::save_game_variables();

  game_variables::set_level_theme( m_theme );
  std::ostringstream stream;
  stream << "level/" << m_serial_number << "/level-" << m_level_number << ".cl";
  
  bear::engine::game::get_instance().push_level( stream.str() );
} // level_selector::push_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Factor of movement to place item on center.
 * \param factor Factor of movement.
 */
void rp::level_selector::on_move_on_center( double factor )
{
  bear::universe::rectangle_type rect( get_level().get_camera_focus() );
    
  bear::universe::position_type dist = 
    rect.bottom_left() + rect.size() / 2 - m_initial_position;
  dist.y += rect.size().y / 15;

  set_center_of_mass( m_initial_position + factor * dist );
} // level_selector::on_move_on_center()

/*----------------------------------------------------------------------------*/
/**
 * \brief Angle of star change.
 * \param angle Angle for star sprite.
 */
void rp::level_selector::on_star_angle_change( double angle )
{
  m_star.set_angle(angle);
} // level_selector::on_star_angle_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update medal picture.
 * \param medal The medal.
 */
void rp::level_selector::update_medal( unsigned int medal )
{
   if ( medal == 3 )
    m_medal_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/medal/medal.png", "bronze" );
  else if ( medal == 4 )
    m_medal_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/medal/medal.png", "silver" );
  else if ( medal == 5 )
    m_medal_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/medal/medal.png", "gold" );
} // level_selector::update_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the level is a boss level.
 */
bool rp::level_selector::is_boss_level() const
{
  return m_level_number == 8;
} // level_selector::is_boss_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the level is the selected level.
 */
bool rp::level_selector::is_selected_level() const
{
  return 
    s_selection && 
    game_variables::get_serial_number() == m_serial_number && 
    game_variables::get_level_number() == m_level_number;
} // level_selector::is_selected_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Init the selected level.
 */
void rp::level_selector::init_selected_level()
{
  game_variables::set_serial_number(m_serial_number);
  game_variables::set_level_number(m_level_number);
  game_variables::set_bronze_threshold(m_bronze_threshold);
  game_variables::set_silver_threshold(m_silver_threshold);
  game_variables::set_gold_threshold(m_gold_threshold); 
  game_variables::set_required_balloons_number(m_required_balloons);
  game_variables::set_level_name(m_level_name);
} // level_selector::init_selected_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Select the level.
 */
void rp::level_selector::select_level()
{
  if ( s_selection )
    return;

  tag_event
    ( "level-selected",
      {
        make_event_property( "locked", "false" ),
        make_event_property( "serial", m_serial_number ),
        make_event_property( "level", m_level_number  ),
        make_event_property
          ( "completed", m_level_state >= detail::state_completed )
      } );

  set_z_position(100);
  s_selection = true;
  game_variables::select_level( true );
  init_selected_level();
  move_on_center();
} // level_selector::select_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief move the item on the center.
 */
void rp::level_selector::move_on_center()
{
  create_opaque_rectangle();
  m_tweeners.clear();
  create_level_name();

  // position
  claw::tween::tweener_sequence tween1;
  tween1.insert
    ( claw::tween::single_tweener
      (0.0, 1.0, 1.0,
       boost::bind
       ( &rp::level_selector::on_move_on_center,
         this, _1 ), &claw::tween::easing_back::ease_in ) );
  m_tweeners.insert(tween1);

  // zoom
  claw::tween::tweener_sequence tween2;
  tween2.insert
    ( claw::tween::single_tweener
      (m_level_factor, 1.0, 1.0,
       boost::bind
       ( &rp::level_selector::on_unlock_factor_change,
         this, _1 ), &claw::tween::easing_back::ease_in ) );
  
  m_tweeners.insert(tween2);
} // level_selector::move_on_center()

/*----------------------------------------------------------------------------*/
/**
 * \brief move the item come back.
 */
void rp::level_selector::come_back()
{
  tag_event
    ( "load-cancelled",
      {
        make_event_property( "serial", m_serial_number ),
        make_event_property( "level", m_level_number  )
      } );
  
  game_variables::set_back_order_status(false);
  game_variables::select_level( false );
  
  start_move_back();
} // level_selector::come_back()

/*----------------------------------------------------------------------------*/
/**
 * \brief check the back order.
 */
void rp::level_selector::start_move_back()
{
  m_medal_movement = 1.0;
  m_tweener_medal =
    claw::tween::single_tweener
    ( m_medal_movement, 1.0, 0.5, &claw::tween::easing_linear::ease_in );

  remove_opaque_rectangle();
  m_tweeners.clear();
  remove_level_name();
  move_back();
} // start_move_back()

/*----------------------------------------------------------------------------*/
/**
 * \brief move the item in the initial position.
 */
void rp::level_selector::move_back()
{
  // position
  claw::tween::tweener_sequence tween1;
  tween1.insert
    ( claw::tween::single_tweener
      (1.0, 0.0, 1.0,
       boost::bind
       ( &rp::level_selector::on_move_on_center,
         this, _1 ), &claw::tween::easing_back::ease_in ) );
  tween1.on_finished
    (boost::bind( &rp::level_selector::stop_selection, this ));  
  m_tweeners.insert(tween1);

  // zoom
  claw::tween::tweener_sequence tween2;
  tween2.insert
    ( claw::tween::single_tweener
      (1.0, m_init_level_factor, 1.0,
       boost::bind
       ( &rp::level_selector::on_unlock_factor_change,
         this, _1 ), &claw::tween::easing_back::ease_in ) );
  
  m_tweeners.insert(tween2);
} // level_selector::move_back()

/*----------------------------------------------------------------------------*/
/**
 * \brief create an opaque rectangle.
 */
void rp::level_selector::create_opaque_rectangle()
{
  m_rectangle = new bear::decorative_rectangle;

  bear::universe::rectangle_type rect( get_level().get_camera_focus() );
  m_rectangle->set_size( rect.size() );
  bear::visual::color c;
  c.set(0,0,0,1.0);
  m_rectangle->set_fill_color( c );    
  m_rectangle->set_z_position( get_z_position() - 5 );
  m_rectangle->set_center_of_mass
    ( rect.bottom_left() + rect.size() / 2 );
  
  new_item(*m_rectangle);

  m_tweener_rectangle_opacity = 
    claw::tween::single_tweener
    ( m_rectangle_opacity, 0.7, 0.5, 
      &claw::tween::easing_linear::ease_in_out );

  if ( get_level().get_camera() != bear::universe::item_handle(NULL) )
    {
      bear::universe::forced_tracking mvt
        ( bear::universe::position_type(0, 0) );
      mvt.set_reference_point_on_center( *(get_level().get_camera().get()) );
      m_rectangle->set_forced_movement( mvt );
    }
} // level_selector::create_opaque_rectangle()

/*----------------------------------------------------------------------------*/
/**
 * \brief create the level name.
 */
void rp::level_selector::create_level_name()
{
  m_decorative_level_name = new bear::decorative_item;

  m_decorative_level_name->set_font
    ( get_level_globals().get_font("font/LuckiestGuy.ttf", 70) );
  m_decorative_level_name->set_text( util::get_level_name() );
  m_decorative_level_name->fit_to_text();
  m_decorative_level_name->set_z_position( get_z_position() - 4 );
  m_decorative_level_name->set_center_of_mass
    ( util::get_level_name_position(get_level().get_camera_focus()) );
  m_decorative_level_name->set_shadow_x(RP_LEVEL_NAME_SHADOW);
  m_decorative_level_name->set_shadow_y(-RP_LEVEL_NAME_SHADOW);
  new_item( *m_decorative_level_name );

  m_level_name_gap_y = get_level().get_camera_focus().size().y / 2;
  m_decorative_level_name->set_gap_y( m_level_name_gap_y );
  
  claw::tween::tweener_sequence t;
  t.insert
    ( claw::tween::single_tweener
      ( m_level_name_gap_y, get_level().get_camera_focus().size().y / 2, 0.5, 
        &claw::tween::easing_back::ease_out ) );
  t.insert
    ( claw::tween::single_tweener
      ( m_level_name_gap_y, 0, 0.5, &claw::tween::easing_back::ease_out ) );
  m_tweeners.insert(t); 
} // level_selector::create_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief remove the opaque rectangle.
 */
void rp::level_selector::remove_opaque_rectangle()
{
  m_rectangle->get_rendering_attributes().set_opacity(m_rectangle_opacity); 

  m_tweener_rectangle_opacity = 
    claw::tween::single_tweener
    ( m_rectangle_opacity, 0.0, 0.5, 
      &claw::tween::easing_linear::ease_in_out );
  m_tweener_rectangle_opacity.on_finished
    (boost::bind( &rp::level_selector::kill_rectangle, this ));
} // level_selector::remove_opaque_rectangle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Kill the rectangle.
 */
void rp::level_selector::kill_rectangle()
{
  if ( m_rectangle != NULL )
    {
      m_rectangle->kill();
      m_rectangle = NULL;
    }
} // kill_rectangle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Kill the decorative item.
 */
void rp::level_selector::kill_level_name()
{
  if ( m_decorative_level_name != NULL )
    {
      m_decorative_level_name->kill();
      m_decorative_level_name = NULL;
    }
} // kill_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief remove the level name.
 */
void rp::level_selector::remove_level_name()
{  
  claw::tween::single_tweener t
    ( claw::tween::single_tweener
      ( m_level_name_gap_y, get_level().get_camera_focus().size().y / 2, 
        0.5, &claw::tween::easing_back::ease_in ) );
  t.on_finished(boost::bind( &rp::level_selector::kill_level_name, this ));
  m_tweeners.insert(t);
} // level_selector::remove_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief check the orders.
 */
void rp::level_selector::check_orders()
{
  check_level_ending();
  check_go_order();
  check_back_order();
} // level_selector::check_orders()

/*----------------------------------------------------------------------------*/
/**
 * \brief check the go order.
 */
void rp::level_selector::check_go_order()
{
  if ( game_variables::get_go_order_status() )
    {
      game_variables::set_back_order_status(false);
      if ( ! game_variables::get_in_loading() )
        {
          load_level();
          if ( m_cursor != NULL )
            m_cursor->get_rendering_attributes().set_opacity(0);
          game_variables::set_in_loading(true);
        }
    }
} // level_selector::check_go_order()

/*----------------------------------------------------------------------------*/
/**
 * \brief check the back order.
 */
void rp::level_selector::check_back_order()
{
  if ( game_variables::get_back_order_status() && 
       m_rectangle != NULL && m_decorative_level_name != NULL )
    come_back();
} // level_selector::check_back_order()

/*----------------------------------------------------------------------------*/
/**
 * \brief check the back order.
 */
void rp::level_selector::check_level_ending()
{
  if ( game_variables::is_level_ending() )
    {
      if ( m_cursor != NULL )
        m_cursor->get_rendering_attributes().set_opacity(1);
      game_variables::set_back_order_status(false);
      game_variables::set_go_order_status(false);

      if ( m_load )
        m_ad_connection =
          show_interstitial
          ( ad_location::level_completed,
            boost::bind( &level_selector::resume, this ) );
    }
}

void rp::level_selector::resume()
{
  m_ad_connection.disconnect();
  
  set_vertical_middle
    ( get_level().get_camera_focus().bottom() + 
      get_level().get_camera_size().y / 2 );
  m_load = false;

  get_level().play_music();

  if ( ! check_fall_medal() )
    {
      start_move_back();
      return;
    }
  
  std::string medal_name;
  
  switch(game_variables::get_last_medal() )
    {
    case 0:
      medal_name = "none";
      break;
    case 1:
      medal_name = "bronze";
      break;
    case 2:
      medal_name = "silver";
      break;
    case 3:
      medal_name = "gold";
      break;
    }
  
  tag_event
    ( "level-completed",
      {
        make_event_property( "medal", medal_name ),
        make_event_property( "serial", m_serial_number ),
        make_event_property( "level", m_level_number  )
      } );
  
  update_state();
}

/*----------------------------------------------------------------------------*/
/**
 * \brief Check if we must fall the medal.
 * return True if there is a better medal. 
 */
bool rp::level_selector::check_fall_medal()
{
  bool result = false;
  unsigned int new_state = game_variables::get_last_medal() + 2;

  if ( new_state >= 3)
    { 
      bear::decorative_item * decorative_medal = new bear::decorative_item;
      std::string medal_name("gold");

      const unsigned int medal( std::min( m_level_state, new_state ) );
        
      if ( medal == detail::state_bronze )
        medal_name = "bronze";
      else if ( medal == detail::state_silver )
        medal_name = "silver";
        
      bear::visual::sprite medal_sprite
        ( get_level_globals().auto_sprite
          ( "gfx/status/medal/medal.png", medal_name ) );
      
      decorative_medal->set_sprite( medal_sprite );
      decorative_medal->set_size( medal_sprite.get_size() );
      decorative_medal->set_z_position( 1000000 );
      decorative_medal->set_kill_when_leaving(true);
      decorative_medal->set_mass(1);

      if ( m_level_state >= new_state ) 
        {
          decorative_medal->set_center_of_mass
            ( util::get_medal_position( get_level().get_camera_focus() ) ); 
          new_item( *decorative_medal );
        }    
      else if ( medal >= detail::state_bronze )
        {
          result = true;
          decorative_medal->set_center_of_mass(get_medal_ending_position());
          new_item( *decorative_medal );
        }
      else
        result = true;
    }

  return result;
} // level_selector::check_fall_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Stop the selection.
 */
void rp::level_selector::stop_selection()
{
  s_selection = false;

  game_variables::set_level_ending(false);   
  game_variables::select_level( false );
  game_variables::set_in_loading(false);  
  game_variables::set_last_medal(0);   

  game_variables::set_level_state
    (m_serial_number, m_level_number, get_new_state());
  m_level_state = 
    game_variables::get_level_state( m_serial_number, m_level_number );
} // level_selector::stop_selection()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the ending position of medal.
 */
bear::visual::position_type 
rp::level_selector::get_medal_ending_position() const
{
  return bear::visual::position_type
    ( get_horizontal_middle() + m_level_factor * m_medal_gap.x, 
      get_vertical_middle() + m_level_factor * m_medal_gap.y );
} // level_selector::get_medal_ending_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the current position of medal.
 */
bear::visual::position_type rp::level_selector::get_medal_position() const
{
  bear::visual::position_type end_position( get_medal_ending_position() );
  
  if ( ! is_selected_level() )
    return end_position;
  else
    {
      bear::visual::position_type start_position =
        util::get_medal_position( get_level().get_camera_focus() );

      bear::visual::position_type dist =
        end_position - start_position;

      return start_position + dist * m_medal_movement;
    }
} // level_selector::get_medal_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Choose the sprite of level.
 */
void rp::level_selector::choose_level_sprite()
{
  if ( (m_level_number == 5)
       || ( (m_level_number == 1) && (m_serial_number == 6) ) )
    m_level_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/level/frame-theme-3.png", "bronze" );
  else if ( (m_level_number == 6)
            || ( (m_level_number == 2) && (m_serial_number == 6) ) )
    m_level_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/level/frame-theme-3.png", "silver" );
  else if ( (m_level_number == 7)
            || ( (m_level_number == 3) && (m_serial_number == 6) ) )
    m_level_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/level/frame-theme-3.png", "gold" );
  else if ( m_theme == "death" || m_theme == "boss") 
    m_level_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/level/frame-theme-2.png", m_theme );
  else
    m_level_sprite =
      get_level_globals().auto_sprite
      ( "gfx/status/level/frame-theme-1.png", m_theme );
} // level_selector::choose_level_theme()
