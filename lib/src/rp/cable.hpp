/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a cable.
 * \author Sebastien Angibaud
 */
#ifndef __RP_CABLE_HPP__
#define __RP_CABLE_HPP__

#include "rp/obstacle.hpp"
#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The class describing a cable.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class cable :
    public obstacle
  {
    DECLARE_BASE_ITEM(cable);
    
  public:
    /** \brief The type of the parent class. */
    typedef obstacle super;

  public:
    cable();
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    bool is_ejected() const;

  private:
    bool collision_with_cart
    (bear::engine::base_item& that, bear::universe::collision_info& info );
    bool collision_with_cable
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    bool collision_with_explosion
    (bear::engine::base_item& that, bear::universe::collision_info& info );
    void eject(bool on_right, bool give_score = true);

  private:
    /* \brief Indicates if the cable has been ejected. */
    bool m_is_ejected;    

    /* \brief Indicates if the cable has already hitted the cart. */
    bool m_has_hit;
  }; // class cable
} // namespace rp

#endif // __RP_CABLE_HPP__
