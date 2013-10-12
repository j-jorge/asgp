/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
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
#include "engine/game_initializer.hpp"
#include "engine/i18n/gettext_translator.hpp"
#include "engine/i18n/android_gettext_translator.hpp"
#include "engine/i18n/translator.hpp"

#include <cstdlib>
#include <ctime>

BEAR_ENGINE_GAME_INIT_FUNCTION( init_super_great_park )

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialization of the game.
 */
void init_super_great_park()
{
  rp::config_file config;
  config.apply();
  config.save();

  const std::string translation_domain_name( "super-great-park" );

#ifdef __ANDROID__
  bear::engine::game::get_instance().set_translator
    ( bear::engine::android_gettext_translator( translation_domain_name ) );
#else
  bear::engine::game::get_instance().set_translator
    ( bear::engine::gettext_translator( translation_domain_name ) );
#endif

  std::srand(time(NULL));
  rp::util::load_game_variables();
  rp::util::send_version();
  rp::game_key::check_if_demo_version();

  claw::logger << claw::log_verbose << "Dumb rendering is "
               << bear::engine::game::get_instance().get_dumb_rendering()
               << std::endl;
} // init_super_great_park()
