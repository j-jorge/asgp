/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \brief Implementation of the functions called at the begining of
 *        the game.
 * \author Julien Jorge
 */
#include "rp/init.hpp"

#include "rp/config_file.hpp"
#include "rp/game_key.hpp"
#include "rp/util.hpp"

#include "engine/game.hpp"
#include "engine/i18n/gettext_translator.hpp"
#include "engine/i18n/translator.hpp"

#include <cstdlib>
#include <ctime>

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialization of the game.
 */
void init_super_great_park()
{
  rp::config_file config;
  config.apply();
  config.save();

  bear::engine::game::get_instance().set_translator
    ( bear::engine::gettext_translator( "super-great-park" ) );

  std::srand(time(NULL));
  rp::util::load_game_variables();
  rp::util::send_version();
  rp::game_key::check_if_demo_version();
} // init_super_great_park()
