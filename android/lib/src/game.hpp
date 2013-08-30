/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The game class handles the game.
 * \author Julien Jorge
 */
#include "engine/game.hpp"

namespace rp
{
  /**
   * \brief The game class handles the game.
   * \author Julien Jorge
   */
  class game
  {
  public:
    game();
    ~game();

    void init();

  private:
    void init_logger();
    void init_game();
    
  private:
    /** \brief The game we run. */
    bear::engine::game* m_game;

  }; // class game
} // namespace rp
