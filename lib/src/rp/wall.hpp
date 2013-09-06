/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a wall.
 * \author Sebastien Angibaud
 */
#ifndef __RP_WALL_HPP__
#define __RP_WALL_HPP__

#include "rp/entity.hpp"

#include "engine/base_item.hpp"
#include "engine/model.hpp"
#include "engine/export.hpp"

#include "generic_items/base_train.hpp"

namespace rp
{
  /**
   * \brief The class describing a wall.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class wall :
    public bear::engine::model < bear::engine::base_item >,
    public bear::base_train,
    public entity
  {
    DECLARE_BASE_ITEM(wall);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model < bear::engine::base_item > super;

  public:
    wall();

    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void hit
    ( const bear::universe::coordinate_type & bottom_y,
      const bear::universe::coordinate_type & top_y );
    
  protected:
    void to_string( std::string& str ) const;
    void move( bear::universe::time_type elapsed_time );
    void populate_loader_map( bear::engine::item_loader_map& m );

  private:
    void get_dependent_items
    ( bear::universe::physical_item::item_list& d ) const;
    bool collision_with_cart
    (bear::engine::base_item& that, bear::universe::collision_info& info);
    bool collision_with_cannonball(bear::engine::base_item& that);
    bool collision_with_tnt(bear::engine::base_item& that);
    bool collision_with_explosion
    (bear::engine::base_item& that, bear::universe::collision_info& info );
    bool collision_with_tar( bear::engine::base_item& that );
    void explose();
    void hit( unsigned int * impacts, const std::string& name);
    void create_decorations
    ( const std::string& name, unsigned int nb_decorations, 
      bool right_orientation );

  private:
    /* \brief Number of impacts on the top section.*/
    unsigned int m_top_impacts;

    /* \brief Number of impacts on the middle section.*/
    unsigned int m_middle_impacts;

    /* \brief Number of impacts on the bottom section.*/
    unsigned int m_bottom_impacts;
  }; // class wall
} // namespace rp

#endif // __RP_WALL_HPP__
