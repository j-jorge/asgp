/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the C++ methods exposed to the Java part.
 * \author Julien Jorge
 */
#include "main.hpp"

#include "game.hpp"

#include <SDL/SDL_main.h>

#include <android/log.h>

/*----------------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{
  for ( int i(0); i != argc; ++i )
    __android_log_print
      ( ANDROID_LOG_INFO, "ASGP Pre", "arg #%d: %s", i, argv[i] );

  rp::game g;

  g.run();

  return 0;
} // main()
