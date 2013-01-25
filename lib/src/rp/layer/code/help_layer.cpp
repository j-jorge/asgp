/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::help_layer class.
 * \author Julien Jorge
 */
#include "rp/layer/help_layer.hpp"
#include "rp/defines.hpp"
#include "rp/game_variables.hpp"
#include "rp/util.hpp"

#include "engine/game.hpp"
#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "gui/picture.hpp"

#include <boost/bind.hpp>

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 * \param name The name of the layer.
 */
rp::help_layer::help_layer( const std::string& name )
  : bear::communication::messageable(name), m_active(false)
{

} // help_layer::help_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the layer.
 */
void rp::help_layer::build()
{
  m_root_window.set_size( get_size() );
  m_root_window.set_background_color( bear::visual::color("#000000") );

  create_component();
  get_level_globals().register_item(*this);
} // help_layer::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that a key has been pressed.
 * \param key The value of the pressed key.
 */
bool rp::help_layer::key_pressed( const bear::input::key_info& key )
{
  bool result = false;

 if ( m_active )
   {
     set_help( false );
     result = true;
   }

  return result;
} // help_layer::key_pressed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the player to start the action associated with a mouse button.
 * \param button The code of the button.
 * \param pos The position of the cursor on the screen.
 */
bool rp::help_layer::mouse_pressed
( bear::input::mouse::mouse_code button,
  const claw::math::coordinate_2d<unsigned int>& pos )
{
  if ( ! m_active )
    return false;
  else
    set_help( false );

  return true;
} // help_layer::mouse_pressed()
  
/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that the mouse had been moved.
 * \param pos The new position of the mouse.
 */
bool rp::help_layer::mouse_move
( const claw::math::coordinate_2d<unsigned int>& pos )
{
  return m_active;
} // help_layer::mouse_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the component that display the "Help" picture.
 */
void rp::help_layer::create_component()
{
  bear::gui::picture* help =
    new bear::gui::picture
    ( get_level_globals().auto_sprite( "gfx/help/help-1.png", "help" ) );

  help->set_left( ( m_root_window.width() - help->width() ) / 2 );
  help->set_bottom( ( m_root_window.height() - help->height() ) / 2  );

  m_root_window.insert( help );
} // help_layer::create_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start to display the help.
 */
void rp::help_layer::start()
{
  if ( ! m_active )
    set_help(true);    
} //  help_layer::start()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets or unsets the help of the level.
 * \param help_on Tells if the level becomes paused.
 */
void rp::help_layer::set_help( bool help_on )
{
  if ( m_active == help_on )
    return;

  if ( help_on )
    {
      set_root_component( &m_root_window );
      get_level().set_pause();
    }
  else
    {
      set_root_component( NULL );
      get_level().unset_pause();
    }

  m_active = help_on;
} // help_layer::set_help()
