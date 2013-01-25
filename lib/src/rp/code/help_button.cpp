/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
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
 * \brief Constructor.
 */
rp::help_button::help_button()
{
} // rp::help_button::rp::help_button()

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

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::help_button::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  progress_input_reader(elapsed_time);
} // help_button::progress()

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
    {
      help_layer_starting_message msg_help;
      
      get_level_globals().send_message
        ( RP_HELP_LAYER_DEFAULT_TARGET_NAME, msg_help );
    }

  return result;
} // help_button::mouse_released

