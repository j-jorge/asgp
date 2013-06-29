/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Game definess.
 * \author Sebastien Angibaud
 */
#ifndef __RP_DEFINES_HPP__
#define __RP_DEFINES_HPP__

/** \brief The prefix for game variable to save. */
#define RP_PERSISTENT_PREFIX "persistent/"

/** \brief The gap of the level name shadow. */
#define RP_LEVEL_NAME_SHADOW 5

/** \brief The name of the layer displaying the transition effects. */
#define RP_TRANSITION_EFFECT_DEFAULT_TARGET_NAME "transition_effect_layer"

/** \brief The name of the layer displaying the balloons the speakers. */
#define RP_BALLOON_LAYER_DEFAULT_TARGET_NAME "balloon_layer"

/** \brief The name of the layer displaying the help. */
#define RP_HELP_LAYER_DEFAULT_TARGET_NAME "help_layer"

/** \brief The name of the layer displaying the key of the user. */
#define RP_KEY_LAYER_DEFAULT_TARGET_NAME "key_layer"

/** \brief The default orange color, used in gui layers. */
#define RP_ORANGE_PIXEL bear::visual::color_type(254, 160, 0, 255)

/** \brief The filename for player save. */
#define RP_SAVE_FILENAME "game-variables.sav"

/** \brief The name of the file containing the key of the game. */
#define RP_KEY_FILE_NAME "key.txt"

#endif // __RP_DEFINES_HPP__
