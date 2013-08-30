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
#include "java_interface.hpp"

#include "game.hpp"

/*----------------------------------------------------------------------------*/
/** \brief The game we are running. */
static rp::game g_game;

/*----------------------------------------------------------------------------*/
/**
 * \brief Initializes the game.
 * \param env The environment running the game.
 */
JNIEXPORT void JNICALL
Java_com_stuffomatic_asgp_ASGP_init( JNIEnv* env )
{
  g_game.init();
} // Java_com_stuffomatic_asgp_ASGP_init()
