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

#include <SDL2/SDL_main.h>

/*----------------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{
  rp::game g;

  g.run();

  exit(0);

  return 0;
} // main()
