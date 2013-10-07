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

#include "rp/util.hpp"

#include "engine/game_initializer.hpp"

BEAR_ENGINE_GAME_END_FUNCTION( end_super_great_park )

void end_super_great_park()
{
  rp::util::save_config();
  rp::util::save_game_variables();
} // end_super_great_park()
