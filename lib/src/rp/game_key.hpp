/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Utility functions to manage the key of a bought game.
 * \author Julien Jorge
 */
#ifndef __RP_GAME_KEY_HPP__
#define __RP_GAME_KEY_HPP__

#include <string>

namespace rp
{
  /**
   * \brief Utility functions to manage the key of a bought game.
   * \author Julien Jorge
   */
  class game_key
  {
  public:
    static bool is_valid_key( std::string key );
    static void check_if_demo_version();

    static void save( std::string key );

  }; // class game_key

} // namespace rp

#endif // __RP_GAME_KEY_HPP__
