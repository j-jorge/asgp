/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::pause_layer class.
 * \author Julien Jorge
 */
#include "rp/layer/pause_layer.hpp"
#include "rp/defines.hpp"
#include "rp/game_variables.hpp"
#include "rp/rp_gettext.hpp"
#include "rp/util.hpp"
#include "rp/message/help_layer_starting_message.hpp"

#include "engine/game.hpp"
#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "engine/stat_variable.hpp"
#include "gui/button.hpp"
#include "gui/callback_function.hpp"
#include "gui/checkbox.hpp"
#include "gui/picture.hpp"
#include "gui/static_text.hpp"
#include "input/key_info.hpp"

#include <boost/bind.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param name The layer will receive the messages targeted to this name.
 */
rp::pause_layer::pause_layer( const std::string& name )
  : bear::communication::messageable( name ), m_margin( 50 ),
    m_active_component( NULL )
{

} // pause_layer::pause_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the layer.
 */
void rp::pause_layer::build()
{
  m_root_window.set_size( get_size() );
  m_root_window.set_background_color( bear::visual::color("#c545485f") );

  add_level_name_component();
  add_title_component();
  add_quit_resume_components();
  add_system_buttons();

  get_level_globals().register_item(*this);
} // pause_layer::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets or unsets the pause of the level.
 * \param pause_on Tells if the level becomes paused.
 */
void rp::pause_layer::set_pause( bool pause_on )
{
  if ( get_level().is_paused() == pause_on )
    return;

  if ( pause_on )
    {
      set_root_component( &m_root_window );
      get_level().set_pause();
    }
  else
    {
      set_root_component( NULL );
      get_level().unset_pause();
    }
} // pause_layer::set_pause()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that a key has been pressed.
 * \param key The value of the pressed key.
 */
bool rp::pause_layer::key_pressed( const bear::input::key_info& key )
{
  bool result = false;

  if ( key.is_escape() || key.is_back() )
   {
     set_pause( !get_level().is_paused() );

     result = true;
   }

  return result;
} // pause_layer::key_pressed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that the mouse had been moved.
 * \param pos The new position of the mouse.
 */
bool rp::pause_layer::mouse_move
( const claw::math::coordinate_2d<unsigned int>& pos )
{
  if ( !get_level().is_paused() )
    return false;

  const claw::math::coordinate_2d<unsigned int> scaled_pos
    ( adjust_screen_position( pos ) );

  bool stop(false);

  for ( std::size_t i=0; !stop && (i!=m_controls.size()); ++i )
    if ( m_controls[i]->get_rectangle().includes(scaled_pos) )
      {
        stop = true;
        highlight_component( m_controls[i] );
      }

  return stop;
} // pause_layer::mouse_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component that display the level name.
 */
void rp::pause_layer::add_level_name_component()
{
  // shadow
  bear::gui::static_text* level_name =
    new bear::gui::static_text
    ( get_level_globals().get_font("font/fontopo/fontopo.fnt",50) );
  level_name->set_auto_size(true);
  level_name->set_text( util::get_level_name() );
  level_name->get_rendering_attributes().set_intensity(0,0,0);

  bear::universe::position_type pos
    ( util::get_level_name_position( m_root_window.get_rectangle() ) );
  level_name->set_left
    ( pos.x - level_name->width() / 2 + RP_LEVEL_NAME_SHADOW );
  level_name->set_top
    ( pos.y + level_name->height() / 2 - RP_LEVEL_NAME_SHADOW );
  m_root_window.insert( level_name );

  // level name
  level_name =
    new bear::gui::static_text
    ( get_level_globals().get_font("font/fontopo/fontopo.fnt",50) );
  level_name->set_auto_size(true);
  level_name->set_text( util::get_level_name() );
  level_name->set_left
    ( pos.x - level_name->width() / 2 );
  level_name->set_top
    ( pos.y + level_name->height() / 2 );

  m_root_window.insert( level_name );
} // pause_layer::add_level_name_component()
  
/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component that display the "Pause" title.
 */
void rp::pause_layer::add_title_component()
{
  bear::gui::picture* title =
    new bear::gui::picture
    ( get_level_globals().auto_sprite( "gfx/status/pause.png", "pause" ) );

  title->set_left( ( m_root_window.width() - title->width() ) / 2 );
  title->set_bottom( m_root_window.height() / 2 + m_margin / 2 );

  m_root_window.insert( title );
} // pause_layer::add_title_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the components to quit or resume the game.
 */
void rp::pause_layer::add_quit_resume_components()
{
  bear::gui::button* quit =
    new bear::gui::button
    ( get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons.png"), "quit" ) );

  bear::gui::button* resume =
    new bear::gui::button
    ( get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons.png"), "resume" ) );

  const bear::gui::size_type horizontal_margin =
    ( m_root_window.width() - 3 * quit->width() ) / 2;

  const bear::gui::coordinate_type top
    ( m_root_window.height() / 2 - quit->height() / 2 );

  quit->set_left( horizontal_margin );
  quit->set_top( top );

  quit->add_callback
    ( bear::gui::callback_function_maker
      ( boost::bind( &pause_layer::on_quit, this ) ) );

  resume->set_right( m_root_window.width() - horizontal_margin );
  resume->set_top( top );

  resume->add_callback
    ( bear::gui::callback_function_maker
      ( boost::bind( &pause_layer::set_pause, this, false ) ) );

  bear::gui::visual_component* strip = new bear::gui::visual_component();
  strip->set_width( m_root_window.width() );
  strip->set_height( quit->height() + m_margin );
  strip->set_background_color( bear::gui::color_type( "#000000" ) );

  strip->set_left( 0 );
  strip->set_bottom
    ( quit->bottom() + ( quit->height() - strip->height() ) / 2 );

  m_root_window.insert( strip );

  add_component( quit );
  add_component( resume );
} // pause_layer::add_quit_resume_components()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the components to turn the music, sound and fullscreen mode on
 *        and off.
 */
void rp::pause_layer::add_system_buttons()
{
  bear::gui::visual_component* help = create_help_component();
  bear::gui::visual_component* sound = create_sound_component();
  bear::gui::visual_component* music = create_music_component();

#if !defined( __ANDROID__ )
  bear::gui::visual_component* fullscreen = create_fullscreen_component();
  const bear::gui::size_type fullscreen_width( fullscreen-»width() );
#else
  const bear::gui::size_type fullscreen_width(0);
#endif

  const bear::gui::size_type horizontal_margin =
    ( m_root_window.width() - help->width() - sound->width() - music->width()
      - fullscreen_width ) / 6;

  help->set_left( horizontal_margin );
  help->set_bottom( m_margin );
  add_component( help );

  sound->set_left( help->right() + 2 * horizontal_margin );
  sound->set_bottom( m_margin );
  add_component( sound );

  music->set_left( sound->right() + horizontal_margin );
  music->set_bottom( m_margin );
  add_component( music );

#if !defined( __ANDROID__ )
  fullscreen->set_left( music->right() + horizontal_margin );
  fullscreen->set_bottom( m_margin );
  add_component( fullscreen );
#endif
} // add_system_buttons()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component to turn the sound on and off.
 */
bear::gui::visual_component* rp::pause_layer::create_sound_component()
{
  bear::gui::checkbox* result = 
    new bear::gui::checkbox
    ( get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons.png"), "sound off" ),
      get_level_globals().auto_sprite
        ( rp_gettext("gfx/status/buttons.png"), "sound on" ) );

  result->add_checked_callback
    ( bear::gui::callback_function_maker
      ( boost::bind
        ( &bear::engine::game::set_sound_muted,
          &bear::engine::game::get_instance(), false ) ) );
  result->add_unchecked_callback
    ( bear::gui::callback_function_maker
      ( boost::bind
        ( &bear::engine::game::set_sound_muted,
          &bear::engine::game::get_instance(), true ) ) );

  result->check( !bear::engine::game::get_instance().get_sound_muted() );

  return result;
} // create_sound_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component to display help.
 */
bear::gui::visual_component* rp::pause_layer::create_help_component()
{
  bear::gui::button* result = 
    new bear::gui::button
    ( get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons-2.png"), "controllers" ) );

  result->add_callback
    ( bear::gui::callback_function_maker
      ( boost::bind( &pause_layer::on_help, this ) ) );
  
  return result;
} // create_sound_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component to turn the music on and off.
 */
bear::gui::visual_component* rp::pause_layer::create_music_component()
{
  bear::gui::checkbox* result = 
    new bear::gui::checkbox
    ( get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons.png"), "music off" ),
      get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons.png"), "music on" ) );

  result->add_checked_callback
    ( bear::gui::callback_function_maker
      ( boost::bind
        ( &bear::engine::game::set_music_muted,
          &bear::engine::game::get_instance(), false ) ) );
  result->add_unchecked_callback
    ( bear::gui::callback_function_maker
      ( boost::bind
        ( &bear::engine::game::set_music_muted,
          &bear::engine::game::get_instance(), true ) ) );

  result->check( !bear::engine::game::get_instance().get_music_muted() );

  return result;
} // create_music_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component to turn the fullscreen mode on and off.
 */
bear::gui::visual_component* rp::pause_layer::create_fullscreen_component()
{
  bear::gui::checkbox* result = 
    new bear::gui::checkbox
    ( get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons.png"), "windowed" ),
      get_level_globals().auto_sprite
      ( rp_gettext("gfx/status/buttons.png"), "fullscreen" ) );

  result->add_checked_callback
    ( bear::gui::callback_function_maker
      ( boost::bind
        ( &bear::engine::game::set_fullscreen,
          &bear::engine::game::get_instance(), true ) ) );
  result->add_unchecked_callback
    ( bear::gui::callback_function_maker
      ( boost::bind
        ( &bear::engine::game::set_fullscreen,
          &bear::engine::game::get_instance(), false ) ) );

  result->check( bear::engine::game::get_instance().get_fullscreen() );

  return result;
} // create_fullscreen_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component that display the focus on the buttons.
 * \param c The component for which the background is created.
 */
void rp::pause_layer::create_focus_background_component
( bear::gui::visual_component* c )
{
  bear::gui::picture* result =
    new bear::gui::picture( get_focus_off_background() );

  result->set_bottom_left( c->left(), c->bottom() );

  m_focus_background[ c ] = result;
  m_root_window.insert( result );
} // pause_layer::create_focus_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the sprite to display behind the focused component.
 */
bear::visual::sprite rp::pause_layer::get_focus_on_background() const
{
  return get_level_globals().auto_sprite
    ( "gfx/status/buttons.png", "background on" );
} // pause_layer::get_focus_off_background()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the sprite to display behind the not focused components.
 */
bear::visual::sprite rp::pause_layer::get_focus_off_background() const
{
  return get_level_globals().auto_sprite
    ( "gfx/status/buttons.png", "background off" );
} // pause_layer::get_focus_off_background()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds a component in the root window and applies the default style to
 *        it.
 * \param c The component to insert.
 */
void rp::pause_layer::add_component( bear::gui::visual_component* c )
{
  create_focus_background_component( c );

  m_root_window.insert( c );
  m_controls.push_back( c );
} // pause_layer::add_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Highlight a component.
 * \param c The component.
 */
void rp::pause_layer::highlight_component( bear::gui::visual_component* c )
{
  if ( m_active_component == c )
    return;

  get_level_globals().play_sound( "sound/tick.ogg" );

  if ( m_active_component != NULL )
    m_focus_background[ m_active_component ]->set_picture
      ( get_focus_off_background() );

  if ( c != NULL )
    m_focus_background[ c ]->set_picture( get_focus_on_background() );

  m_active_component = c;
} // pause_layer::highlight_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Loads the level selector.
 */
void rp::pause_layer::on_quit()
{
  game_variables::set_level_ending(true);

  util::send_quit_level(get_level().get_filename());

  bear::engine::game::get_instance().pop_level();
} // pause_layer::on_quit()

/*----------------------------------------------------------------------------*/
/**
 * \brief Display the help.
 */
void rp::pause_layer::on_help()
{
  help_layer_starting_message msg_help;
      
  get_level_globals().send_message
    ( RP_HELP_LAYER_DEFAULT_TARGET_NAME, msg_help );
} // pause_layer::on_help()
