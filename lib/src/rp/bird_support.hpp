/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a support for birds.
 * \author Sebastien Angibaud
 */
#ifndef __RP_BIRD_SUPPORT_HPP__
#define __RP_BIRD_SUPPORT_HPP__

#include "generic_items/decorative_item.hpp"

namespace rp
{
  /**
   * \brief The class describing a support for birds.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class bird_support :
    public bear::decorative_item
  {
    DECLARE_BASE_ITEM(bird_support);

  public:
    /** \brief The type of the parent class. */
    typedef bear::decorative_item super;

  public:
    bird_support();

    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

  private:
    bool collision_with_cart
    (bear::engine::base_item& that, bear::universe::collision_info& info);
  }; // class bird_support
} // namespace rp

#endif // __RP_BIRD_SUPPORT_HPP__
