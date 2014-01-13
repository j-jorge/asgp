/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::game class.
 * \author Julien Jorge
 */
#include "game.hpp"

#include "engine/resource_pool.hpp"

#include "engine/resource_pool/android_resource_pool.hpp"

#include "engine/system/game_filesystem.hpp"

#include <claw/android_logger.hpp>
#include <claw/logger.hpp>
#include <claw/log_stream_uniq.hpp>

#include <SDL2/SDL.h>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructs un empty game.
 */
rp::game::game()
  : m_game(NULL)
{

} // game::game()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destroys the game and the managed ressources.
 */
rp::game::~game()
{
  delete m_game;
} // game::game()

/*----------------------------------------------------------------------------*/
/**
 * \brief Runs the game.
 */
void rp::game::run()
{
  init();

  try
    {
      m_game->run();
    }
  catch( std::exception& e )
    {
      claw::logger << claw::log_error << "Exit on exception: " << e.what()
                   << std::endl;
    }

  SDL_Quit();
} // game::run()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initializes the game.
 */
void rp::game::init()
{
  delete m_game;

  init_logger();

  claw::logger << claw::log_verbose << "Initializing game instance."
               << std::endl;

  init_game();
} // game::init()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initializes the log system.
 */
void rp::game::init_logger()
{
  claw::log_stream* const logger
    ( new claw::log_stream_uniq( new claw::android_logger( "ASGP" ) ) );
  
  claw::logger.set( logger );
  claw::logger.set_level( claw::log_verbose );
} // game::init_logger()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initializes the underlying game.
 */
void rp::game::init_game()
{
  const char* default_args[] =
    {
      "--game-name=Super Great Park",
      "--screen-width=1280",
      "--screen-height=720",
      "--fullscreen",
      "--active-area=250",
      "--set-game-var-bool=demo_version=0",
      "--tag=android",
      "--auto-load-symbols",
      "--stats-destination=http://www.stuff-o-matic.com/asgp/stats/save.php",
      "--start-level=level/start.cl",
      NULL
    };

  // Compute the number of elements in default_args
  int default_argc(0);
  while ( default_args[ default_argc ] != NULL )
    ++default_argc;

  try
    {
      char** engine_args = const_cast<char**>(default_args);
      m_game = new bear::engine::game( default_argc, engine_args );

      bear::engine::resource_pool::get_instance().add_pool
        ( new bear::engine::android_resource_pool );                     
    }
  catch( std::exception& e )
    {
      claw::logger << claw::log_error << "Exception: " << e.what() << std::endl;
    }
} // game::init_game()
