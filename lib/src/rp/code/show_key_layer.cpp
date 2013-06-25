/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the show_key_layer class.
 * \author Sebastien Angibaud
 */
#include "rp/show_key_layer.hpp"

#include "rp/defines.hpp"

#include "rp/message/key_layer_starting_message.hpp"

#include "engine/level_globals.hpp"

BASE_ITEM_EXPORT( show_key_layer, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::show_key_layer::show_key_layer()
: m_layer_name( RP_KEY_LAYER_DEFAULT_TARGET_NAME )
{
  set_phantom( true );
  set_can_move_items( false );
  set_artificial( true );
} // rp::show_key_layer::show_key_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief The toggle is activated, kill the player.
 * \param activator The item who activated this toggle.
 */
void rp::show_key_layer::on_toggle_on
( bear::engine::base_item* activator )
{
  super::on_toggle_on(activator);

  key_layer_starting_message msg;
      
  get_level_globals().send_message( m_layer_name, msg );
} // show_key_layer::on_toggle_on()
