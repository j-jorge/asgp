/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The main class of the application.
 * \author Julien Jorge
 */
#ifndef __BEAR_LAUNCHER_HPP__
#define __BEAR_LAUNCHER_HPP__

#include <claw/application.hpp>
#include "engine/game.hpp"

namespace rp
{
  /**
   * \brief The main class of the application.
   * \author Julien Jorge
   */
  class launcher:
    public claw::application
  {
  public:
    launcher( int& argc, char** &argv );
    virtual ~launcher();

    virtual int run();

  private:
    void create_game( int& argc, char** &argv );
    void help() const;

    std::string get_application_path() const;
    std::string get_data_path_argument() const;
    std::string get_generic_items_argument() const;
    std::string get_rp_argument() const;

  private:
    /** \brief The game we are running. */
    bear::engine::game* m_game;

  }; // class launcher
} // namespace bear

#endif // __BEAR_LAUNCHER_HPP__
