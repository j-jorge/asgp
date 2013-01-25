/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a tnt.
 * \author Sebastien Angibaud
 */
#ifndef __RP_TNT_HPP__
#define __RP_TNT_HPP__

#include "rp/obstacle.hpp"
#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The class describing a tnt.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class tnt :
    public obstacle
  {
    DECLARE_BASE_ITEM(tnt);
    
  public:
    /** \brief The type of the parent class. */
    typedef obstacle super;

    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
  public:
    tnt();

    void pre_cache();
    void on_enters_layer();
    
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void explose(bool give_score = true);
    
  private:
    bool collision_with_cart(bear::engine::base_item& that);
    bool collision_with_cannonball(bear::engine::base_item& that);
    bool collision_with_bird(bear::engine::base_item& that);
    bool collision_with_explosion
    (bear::engine::base_item& that, bear::universe::collision_info& info );
    bool collision_with_bomb(bear::engine::base_item& that);
    bool collision_with_obstacle(bear::engine::base_item& that);
    bool collision_with_plank(bear::engine::base_item& that);
    bool collision_with_zeppelin(bear::engine::base_item& that);
    void finish_explose();
    void create_explosion
    (unsigned int nb_explosions, bear::universe::coordinate_type radius);
    void create_second_explosion();
    void create_third_explosion();

  private:
    static void init_exported_methods();

  private:
    /** \brief Indicates if the bomb has been explosed. */
    bool m_explosed;
  }; // class tnt
} // namespace rp

#endif // __RP_TNT_HPP__
