/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a bird.
 * \author Sebastien Angibaud
 */
#ifndef __RP_BIRD_HPP__
#define __RP_BIRD_HPP__

#include "rp/entity.hpp"

#include "engine/model.hpp"
#include "engine/base_item.hpp"
#include "generic_items/decorative_item.hpp"
#include "generic_items/reference_item.hpp"

namespace rp
{
  /**
   * \brief The class describing a bird.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class bird :
    public bear::engine::model< bear::engine::base_item >,
    public entity
  {
    DECLARE_BASE_ITEM(bird);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model< bear::engine::base_item >  super;
  
    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
    private:
    typedef void (bird::*progress_function_type)(bear::universe::time_type);
  
  public:
    bird();
    
    void pre_cache();
    void on_enters_layer();
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void afraid(bool give_points = true);
    void plunger_collision();
 
  protected:
    void populate_loader_map( bear::engine::item_loader_map& m );
    
  private:
    bool collision_with_cannonball(bear::engine::base_item& that);
    bool collision_with_explosion
    (bear::engine::base_item& that, bear::universe::collision_info& info);
    bool collision_with_cart(bear::engine::base_item& that);
    bool collision_with_cable(bear::engine::base_item& that);
    bool collision_with_crate
    (bear::engine::base_item& that, bear::universe::collision_info& info);
    bool collision_with_plank(bear::engine::base_item& that);
    bool collision_with_wall(bear::engine::base_item& that);
    bool collision_with_tar(bear::engine::base_item& that);
    bool collision_with_balloon(bear::engine::base_item& that);
    bool collision_with_bird
    (bear::engine::base_item& that, bear::universe::collision_info& info);
    bool collision_with_zeppelin
    (bear::engine::base_item& that, bear::universe::collision_info& info);

    // start functions
    void start_fly();
    void start_hit();
    void make_dirty();
 
    void on_beak_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void create_explosion_feathers();
    void create_feathers();
    void create_feather();
    void set_feather_anchor_reference(bear::reference_item* ref);
    void set_feather_reference(bear::reference_item* ref);
    void set_feather(bear::decorative_item* item);

  private:
    static void init_exported_methods();

  private:
    /* \brief Number of plunger's collision */
    unsigned int m_plunger_collision;
  }; // class bird
} // namespace rp

#endif // __RP_BIRD_HPP__
