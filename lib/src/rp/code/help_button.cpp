/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the help_button class.
 * \author Sebastien Angibaud
 */
#include "rp/help_button.hpp"
#include "rp/defines.hpp"
#include "rp/message/help_layer_starting_message.hpp"
#include "rp/rp_gettext.hpp"

#include "engine/level_globals.hpp"
#include "visual/scene_sprite.hpp"

BASE_ITEM_EXPORT( help_button, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::help_button::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_image
    (rp_gettext("gfx/status/buttons.png"));
  get_level_globals().load_image
    (rp_gettext("gfx/status/buttons-2.png"));
} // rp::help_button::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::help_button::on_enters_layer()
{
  super::on_enters_layer();

  m_sprite =
    get_level_globals().auto_sprite
    ( rp_gettext("gfx/status/buttons-2.png"),"controllers");
} // help_button::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the sprite representing the item.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::help_button::get_visual
( std::list<bear::engine::scene_visual>& visuals ) const
{
  super::get_visual(visuals);
  
  bear::visual::scene_sprite s
    ( get_left(), get_bottom(), m_sprite );
  visuals.push_back( s );
} // help_button::get_visuals()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the player to stop the action associated with a mouse button.
 * \param button The code of the button.
 * \param pos The position of the cursor on the screen.
 */
bool rp::help_button::mouse_released
( bear::input::mouse::mouse_code button,
  const claw::math::coordinate_2d<unsigned int>& pos )
{
  bool result = super::mouse_released(button, pos);
  
  if ( result )
    show_help();

  return result;
} // help_button::mouse_released()

/*----------------------------------------------------------------------------*/
/**
 * \brief Shows the help if a finger has been released on the button.
 * \param event The event dispatched by the finger.
 */
bool rp::help_button::finger_action
( const bear::input::finger_event& event )
{
  bool result = super::finger_action( event );
  
  if ( result
       && ( event.get_type()
            == bear::input::finger_event::finger_event_released ) )
    show_help();

  return result;
} // help_button::finger_action()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the player to stop the action associated with a mouse button.
 * \param button The code of the button.
 * \param pos The position of the cursor on the screen.
 */
void rp::help_button::show_help() const
{
  help_layer_starting_message msg_help;
      
  get_level_globals().send_message
    ( RP_HELP_LAYER_DEFAULT_TARGET_NAME, msg_help );
} // help_button::show_help()
