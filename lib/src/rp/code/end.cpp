/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \brief Implementation of the functions called at the end of the game.
 * \author Sébastien Angibaud
 */
#include "rp/end.hpp"

#include "rp/config_file.hpp"
#include "rp/util.hpp"
#include "engine/game.hpp"

void end_super_great_park()
{
  rp::config_file config;
  config.update();
  config.save();

  rp::util::save_game_variables();
} // end_super_great_park()
