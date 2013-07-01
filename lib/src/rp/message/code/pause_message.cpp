/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::pause_message class.
 * \author Julien Jorge
 */
#include "rp/message/pause_message.hpp"

#include "rp/layer/pause_layer.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply the message to the pause layer.
 * \param that The balloon layer to apply the message to.
 */
bool rp::pause_message::apply_to( pause_layer& that )
{
  that.set_pause( true );

  return true;
} // pause_gmessage::apply_to()
