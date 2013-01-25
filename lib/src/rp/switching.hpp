/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief A switching on the track
 * \author Julien Jorge
 */
#ifndef __RP_SWITCHING_HPP__
#define __RP_SWITCHING_HPP__

#include "rp/entity.hpp"
#include "engine/model.hpp"
#include "engine/base_item.hpp"
#include "engine/export.hpp"

#include "generic_items/slope.hpp"

namespace rp
{
  class hole;

  /**
   * \brief The class describing a switching.
   *
   * The valid fields for this item are
   *  - switching.initial_state (string) the initial direction of the switching
   *    (default = "up"),
   *  - switching.up.steepness (real) the steepness of the up direction
   *    (default = 64),
   *  - switching.down.steepness (real) the steepness of the down direction
   *    (default = -50),
   *  - switching.up.z (int) the z position of the up direction (default = 0),
   *  - switching.down.z (int) the z position of the down direction
   *    (default = 1000),
   *  - any field supported by the parent classes.
   *
   * \author Julien Jorge
   */
  class switching :
    public bear::engine::model< bear::engine::base_item >,
    public entity
  {
    DECLARE_BASE_ITEM(switching);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model< bear::engine::base_item > super;

    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
  public:
    switching();

    // not implemented
    switching( const switching& that ) {};

    ~switching();

    void pre_cache();
    bool is_valid();
    void on_enters_layer();

    bool set_string_field( const std::string& name, const std::string& value );
    bool set_real_field( const std::string& name, double value );
    bool set_integer_field( const std::string& name, int value );
    void enters_active_region();

  protected:
    void populate_loader_map( bear::engine::item_loader_map& m );

  private:
    void init_slope( bear::slope* & slope, double s, int z );
    
    // start functions
    void on_up();
    void on_down();
    void on_move_up();
    void on_move_down();
    
    // events
    void on_switch_down
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_switch_up
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_switch
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info, const std::string& action );

    // other functions
    void init_hole_block();
    void remove_hole_block();

  private:
    static void init_exported_methods();

  private:
    /** \brief The initial state of the switching. */
    std::string m_initial_state;
    
    /** \brief The steepness to go upward. */
    double m_steepness_up;

    /** \brief The z position of the up path. */
    int m_z_up;

    /** \brief The slope to go upward. */
    bear::slope* m_up_slope;

    /** \brief The steepness to go downward. */
    double m_steepness_down;

    /** \brief The z position of the down path. */
    int m_z_down;

    /** \brief The slope to go downward. */
    bear::slope* m_down_slope;

    /** \brief The block that kills the cart when the switching is moving. */
    hole* m_hole;

  }; // class switching
} // namespace rp

#endif // __RP_SWITCHING_HPP__
