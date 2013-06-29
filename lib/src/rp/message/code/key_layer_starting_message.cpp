/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::key_layer_starting_message class.
 * \author Julien Jorge
 */
#include "rp/message/key_layer_starting_message.hpp"

#include "rp/layer/key_layer.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply the message to the key layer.
 * \param that The balloon layer to apply the message to.
 */
bool rp::key_layer_starting_message::apply_to( key_layer& that )
{
  that.show();

  return true;
} // key_layer_starting_gmessage::apply_to()
