/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a plunger.
 * \author Sebastien Angibaud
 */
#ifndef __RP_PLUNGER_HPP__
#define __RP_PLUNGER_HPP__

#include "engine/model.hpp"
#include "engine/base_item.hpp"
#include "engine/export.hpp"

#include "universe/derived_item_handle.hpp"

namespace rp
{
  class cart;
  class balloon;
  class attractable_item;

  /**
   * \brief The class describing a plunger.
   * \author Sebastien Angibaud
   */
  class plunger : 
    public bear::engine::model< bear::engine::base_item >
  {
    DECLARE_BASE_ITEM(plunger);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model< bear::engine::base_item > super;

    /** \brief The type of an handle on a attracted_item. */
    typedef
    bear::universe::derived_item_handle<attractable_item> handle_type;

    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)

  public:
    plunger();
    
    void pre_cache();
    void on_enters_layer();    
    void progress( bear::universe::time_type elapsed_time );

    void set_cart(cart* c);
    cart* get_cart();
    bear::universe::position_type get_tail_position() const;
    void leave();

    void create_movement
    ( const bear::universe::vector_type& dir, 
      const bear::universe::speed_type& speed);
    void stop(bool bounce);
    bool come_back() const;
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

  private:
    bool collision_with_attractable( bear::engine::base_item& that );
    bool collision_with_zeppelin( bear::engine::base_item& that );
    bool collision_with_bird( bear::engine::base_item& that );
    bool collision_with_obstacle( bear::engine::base_item& that );
    bool collision_with_wall( bear::engine::base_item& that );
    void create_attracted_movement(bear::engine::base_item& that);
    void create_back_movement(bool bounce);
    void on_plunger_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void update_angle();
    void update_plunger_position(bear::universe::time_type elapsed_time);   
    void attract_cart(bear::engine::base_item& that);

  private:
    static void init_exported_methods();

  public:
    /** \brief The maximum distance between plunger and cart. */
    static const double s_max_plunger_distance;

    /** \brief The maximum distance between plunger and cart in boss level. */
    static const double s_max_boss_plunger_distance;

  private:
    /** \brief A pointer on cart. */
    cart* m_cart;   

    /** \brief Indicates that the plunger come back. */
    bool m_come_back;   

    /** \brief A pointer on attracted item. */
    handle_type m_attracted_item;   
    
    /** \brief The angle of the plunger. */
    double m_angle;

    /** \brief The maximum distance between plunger and cart. */
    double m_max_plunger_distance;

    /** \brief The rectangle of the camera. */
    bear::universe::rectangle_type m_camera_rect;
  }; // class plunger
} // namespace rp

#endif // __RP_PLUNGER_HPP__
