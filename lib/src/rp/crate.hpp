/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a crate.
 * \author Sebastien Angibaud
 */
#ifndef __RP_CRATE_HPP__
#define __RP_CRATE_HPP__

#include "rp/obstacle.hpp"
#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The class describing a crate.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class crate :
    public obstacle
  {
    DECLARE_BASE_ITEM(crate);
    
  public:
    /** \brief The type of the parent class. */
    typedef obstacle super;

  public:
    crate();

    void pre_cache();
    void on_enters_layer();
    
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

  private:
    bool collision_with_cart(bear::engine::base_item& that);
    bool collision_with_cannonball(bear::engine::base_item& that);
    bool collision_with_cable(bear::engine::base_item& that);
    bool collision_with_explosion
    (bear::engine::base_item& that, bear::universe::collision_info& info );
    void explose(bool give_score = true);
  }; // class crate
} // namespace rp

#endif // __RP_CRATE_HPP__
