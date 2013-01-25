/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a level generator.
 * \author Sebastien Angibaud
 */
#ifndef __RP_LEVEL_GENERATOR_HPP__
#define __RP_LEVEL_GENERATOR_HPP__

#include "rp/obstacle.hpp"

#include "engine/base_item.hpp"
#include "generic_items/slope.hpp"
#include "generic_items/straight_slope.hpp"
#include "universe/derived_item_handle.hpp"

#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The class describing a level generator.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class level_generator :
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM(level_generator);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::base_item super;

  private:
    /** \brief The type of the handles on the slopes. */
    typedef bear::universe::derived_item_handle<bear::slope> slope_handle;
    
    /** \brief The type of the vector of the handles to the slopes. */
    typedef std::vector<slope_handle> slope_vector;

    /** \brief The type of the handles on the straight slopes. */
    typedef bear::universe::derived_item_handle<bear::straight_slope> 
    straight_slope_handle;
    
    /** \brief The type of the vector of the handles to the straight slopes. */
    typedef std::vector<straight_slope_handle> straight_slope_vector;    

    /** \brief The type of the handles on the obstacle. */
    typedef bear::universe::derived_item_handle<rp::obstacle> 
    obstacle_handle;
    
    /** \brief The type of the vector of the handles to the obstacles. */
    typedef std::vector<obstacle_handle> obstacle_vector; 
    
  public:
    void on_enters_layer();

    bool set_item_list_field
    ( const std::string& name, 
      const std::vector<bear::engine::base_item*>& value );
    bool set_item_field
    ( const std::string& name, bear::engine::base_item* value );

  private:
    void create_level();
    void create_target(bear::universe::position_type& pos);
    void create_bonus(bear::universe::position_type& pos);
    void add_slope(bear::universe::position_type& pos);
    void add_ground(bear::universe::position_type& pos);
    void add_hole(bear::universe::position_type& pos);
    void add_obstacle(bear::universe::position_type& pos);
    void add_straight_slope(bear::universe::position_type& pos);

  private:
    /** \brief A vector of slope to build level. */
    slope_vector m_slopes;

    /** \brief A vector of straight_slope to build level. */
    straight_slope_vector m_straight_slopes;

    /** \brief A straight_slope for right hole. */
    straight_slope_handle m_right_hole;
    
    /** \brief A straight_slope for left hole. */
    straight_slope_handle m_left_hole;

    /** \brief A vector of obstacles to build level. */
    obstacle_vector m_obstacles;
  }; // class level_generator
} // namespace rp

#endif // __RP_LEVEL_GENERATOR_HPP__
