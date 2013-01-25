/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::help_layer_starting_message class.
 * \author Sebastien Angibaud
 */
#include "rp/message/help_layer_starting_message.hpp"
#include "rp/layer/help_layer.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::help_layer_starting_message::help_layer_starting_message()
{

} // help_layer_starting_message::help_layer_starting_message()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply the message to the balloon layer.
 * \param that The balloon layer to apply the message to.
 */
bool rp::help_layer_starting_message::apply_to( help_layer& that )
{
  that.start();

  return true;
} // help_layer_startin_gmessage::apply_to()
