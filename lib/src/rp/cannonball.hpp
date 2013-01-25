/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a cannonball.
 * \author Sebastien Angibaud
 */
#ifndef __RP_CANNONBALL_HPP__
#define __RP_CANNONBALL_HPP__

#include "engine/model.hpp"
#include "engine/base_item.hpp"
#include "engine/export.hpp"
#include "generic_items/reference_item.hpp"

namespace rp
{
  class cart;

  /**
   * \brief The class describing a cannonball.
   * \author Sebastien Angibaud
   */
  class cannonball :
    public bear::engine::model
  < bear::engine::base_item >
  {
    DECLARE_BASE_ITEM(cannonball);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model< bear::engine::base_item > super;

  public:
    cannonball();
    
    void pre_cache();
    void on_enters_layer();    
    void progress( bear::universe::time_type elapsed_time );
    void kill();
    void create_movement
    ( const bear::universe::vector_type& dir, 
      const bear::universe::speed_type& cart_speed,
      const bear::universe::position_type& sight_pos, bool good_fire_angle);
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void set_cart(cart* c);
    cart* get_cart();
    unsigned int get_combo_value() const;

  private:
    bool collision_with_cable
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    bool collision_with_tar( bear::engine::base_item& that);

    void create_trace() const;

  private:
    /** \brief A reference point on sight. */
    bear::reference_item* m_sight_position;

    /** \brief A pointer on cart. */
    cart* m_cart;  
    
    /* \brief Value of the combo. */
    unsigned int m_combo_value;
  }; // class cannonball
} // namespace rp

#endif // __RP_CANNONBALL_HPP__
