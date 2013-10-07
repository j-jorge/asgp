/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::config_save class.
 * \author Julien Jorge
 */
#include "rp/config_save.hpp"

#include "rp/util.hpp"

BASE_ITEM_EXPORT( config_save, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Saves the config.
 * \param activator The item which activated the toggle.
 */
void rp::config_save::on_toggle_on( bear::engine::base_item* activator )
{
  util::save_config();
} // config_save::on_toggle_on()
