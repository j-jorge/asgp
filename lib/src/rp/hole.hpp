/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a hole.
 * \author Sebastien Angibaud
 */
#ifndef __RP_HOLE_HPP__
#define __RP_HOLE_HPP__

#include "engine/base_item.hpp"
#include "engine/export.hpp"

namespace rp
{
  class cart;

  /**
   * \brief The class describing a hole.
   *²
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class hole :
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM(hole);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::base_item super;

  public:

    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
  }; // class hole
} // namespace rp

#endif // __RP_HOLE_HPP__
