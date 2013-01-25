/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief An item that changes the level when a player is colliding with it.
 * \author Sébastien Angibaud
 */
#ifndef __RP_LEVEL_EXIT_HPP__
#define __RP_LEVEL_EXIT_HPP__

#include "engine/base_item.hpp"

#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief Items of this class start a level when all players are in collision
   *        with one of them.
   * \author Sébastien Angibaud
   */
  class level_exit:
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM(level_exit);

  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::base_item super;

  public:
    level_exit();

    void build();

  private:
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

  }; // class level_exit
} // namespace rp

#endif // __RP_LEVEL_EXIT_HPP__
