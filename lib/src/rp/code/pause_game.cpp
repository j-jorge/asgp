/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the pause_game class.
 * \author Sebastien Angibaud
 */
#include "rp/pause_game.hpp"

#include "rp/defines.hpp"

#include "rp/message/pause_message.hpp"

#include "engine/level_globals.hpp"

BASE_ITEM_EXPORT( pause_game, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::pause_game::pause_game()
: m_layer_name( RP_PAUSE_LAYER_DEFAULT_TARGET_NAME )
{
  set_phantom( true );
  set_can_move_items( false );
  set_artificial( true );
} // rp::pause_game::pause_game()

/*----------------------------------------------------------------------------*/
/**
 * \brief The toggle is activated, pause the gamex.
 * \param activator The item who activated this toggle.
 */
void rp::pause_game::on_toggle_on
( bear::engine::base_item* activator )
{
  super::on_toggle_on(activator);

  pause_message msg;
      
  get_level_globals().send_message( m_layer_name, msg );
} // pause_game::on_toggle_on()
