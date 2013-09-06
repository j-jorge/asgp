/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing an obstacle.
 * \author Sebastien Angibaud
 */
#ifndef __RP_OBSTACLE_HPP__
#define __RP_OBSTACLE_HPP__

#include "rp/entity.hpp"

#include "engine/model.hpp"
#include "engine/base_item.hpp"
#include "generic_items/base_train.hpp"
#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The class describing an obstacle.
   *
   * The valid fields for this item are
   * -  random_explosion (bool) Indicates if the explosion has random 
   * parameters(default : true)
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class obstacle :
    public bear::engine::model< bear::engine::base_item >,
    public bear::base_train,
    public entity
  {
    DECLARE_BASE_ITEM(obstacle);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model< bear::engine::base_item > super;

  public:
    obstacle();

    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    bool set_bool_field( const std::string& name, bool value );

  protected:
    void to_string( std::string& str ) const;
    void move( bear::universe::time_type elapsed_time );
    void create_planks(const std::string& mark_name);
    void populate_loader_map( bear::engine::item_loader_map& m );

  private:
    void create_random_planks(const std::string& mark_name);
    void create_fixed_planks(const std::string& mark_name);
    void get_dependent_items
    ( bear::universe::physical_item::item_list& d ) const;
    bool collision_with_tar( bear::engine::base_item& that );
    void make_dirty();
    void create_plank
    (const std::string& mark_name, const bear::universe::force_type& force);

  private:
    /* \brief Indicates if the explosion has random parameters. */
    bool m_random_explosion;
  }; // class obstacle
} // namespace rp

#endif // __RP_OBSTACLE_HPP__
