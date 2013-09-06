/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a boss.
 * \author Sebastien Angibaud
 */
#ifndef __RP_BOSS_HPP__
#define __RP_BOSS_HPP__

#include "rp/entity.hpp"
#include "rp/item_that_speaks.hpp"

#include "engine/model.hpp"
#include "engine/base_item.hpp"
#include "generic_items/reference_item.hpp"
#include "universe/derived_item_handle.hpp"

#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/single_tweener.hpp>

namespace rp
{
  class cart;
  class cannonball;

  /**
   * \brief The class describing a boss.
   *
   * The valid fields for this item are
   * - cart: (item) A pointer on the cart,
   * - transition_mode (boolean) Indicates if the boss is in a transition level.
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class boss :
    public bear::engine::model
  < item_that_speaks< bear::engine::base_item > >,
    public entity
  {
    DECLARE_BASE_ITEM(boss);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model
    < item_that_speaks< bear::engine::base_item > >  super;
  
    /** \brief The type of an handle on a reference_item. */
    typedef
    bear::universe::derived_item_handle<bear::engine::base_item> handle_type;

    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
    private:
    typedef void (boss::*progress_function_type)(bear::universe::time_type);

  public:
    boss();
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    bool set_u_integer_field( const std::string& name, unsigned int value );
    bool set_item_field( const std::string& name, base_item* value );
    bool is_valid() const;
    void follow_cart_bottom();
    void set_y_reference(bear::universe::coordinate_type pos_y);
    void set_teleportation_gap(const bear::universe::position_type& gap);
    handle_type set_drop_item(base_item* item, base_item* zeppelin_item);
    void get_dependent_items
    ( bear::universe::physical_item::item_list& d ) const;

  private:
    void remove_drop_items();
    void progress_fly( bear::universe::time_type elapsed_time );
    void progress_dead( bear::universe::time_type elapsed_time );
    void progress_injured_state();
    void progress_drop( bear::universe::time_type elapsed_time );
    void progress_injure( bear::universe::time_type elapsed_time );
    void progress_end( bear::universe::time_type elapsed_time );
    void progress_drop_item();
    void progress_tweeners( bear::universe::time_type elapsed_time );
    void update_interactive_item();

    void start_fly();
    void start_dead();
    void start_injure();
    void start_end();
    void apply_fly();
    void apply_dead();
    void apply_injure();
    void apply_end();

    bool collision_with_plunger( bear::engine::base_item& that );
    bool collision_with_cart( bear::engine::base_item& that );
    void collision_with_cannonball
    ( cannonball& c, const bear::universe::speed_type& oriented_speed );
    bool collision_with_cannonball
    ( bear::engine::base_item& that,
      bear::universe::collision_info& info );

    void on_cabin_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_bottom_cabin_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_left_cabin_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_trap_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_button_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_propeller_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_emergency_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );

    void update_trap_door( bear::universe::time_type elapsed_time );
    void close_trap_door();
    void open_trap_door();
    void update_angle();
    void update_safe_angle();
    void update_injured_angle();
    void create_anchor();
    void drop();
    void init_teleportation_gap();

    void create_transition_first_movement();
    void create_transition_second_movement();
    void start_transition_third_movement();
    void create_transition_third_movement();
    void create_initial_anchor_movement();
    void create_initial_anchor_y_movement(double initial_value);
    void create_initial_anchor_y_movement();
    void create_fly_anchor_movement();
    void create_drop_anchor_movement();
    void create_dead_anchor_movement();
    void create_dead_movement();    
    void create_final_movement();
    void on_anchor_transition_x_change
    (const bear::universe::coordinate_type& value);
    void on_anchor_transition_y_change
    (const bear::universe::coordinate_type& value);
    void on_anchor_ratio_x_change(const bear::universe::coordinate_type& value);
    void on_anchor_x_change(const bear::universe::coordinate_type& value);
    void on_anchor_ratio_y_change(const bear::universe::coordinate_type& value);
    void on_anchor_y_reference_change
    (const bear::universe::coordinate_type& value);
    void on_anchor_y_change(const bear::universe::coordinate_type& value);
    void on_angle_change(double value);
    void on_movement_ratio_change(const bear::universe::coordinate_type& value);
    void create_fly_movement();
    void create_fly_transition_movement();
    void choose_module( unsigned int serial );
    void hit();
    void throw_element(const std::string& mark_name);
    void create_interactive_item();
    void create_hit_star();
    void init_transition_mode();
    void restore_module();

  private:
    static void init_exported_methods();

  private:
    /** \brief Current progress function. */
    progress_function_type m_progress;

    /** \brief A pointer on the cart. */
    rp::cart* m_cart;

    /** \brief A pointer on a reference_item for forced movement. */
    bear::reference_item* m_anchor;

    /** \brief The tweener for system_angle. */
    claw::tween::tweener_sequence m_tweener_angle;

    /** \brief The tweener for anchor on x-coordinate. */
    claw::tween::tweener_sequence m_tweener_anchor_x;
    
    /** \brief The second tweener for anchor on y-coordinate. */
    claw::tween::tweener_sequence m_tweener_anchor_y;

    /** \brief The tweener for y-coordinate reference. */
    claw::tween::tweener_sequence m_tweener_reference_y;

    /** \brief The tweener for movement. */
    claw::tween::tweener_sequence m_tweener_movement;

    /** \brief Initial position of anchor in the current movement. */
    bear::universe::position_type m_initial_anchor_position;
    
    /** \brief Movement of anchor in the current movement. */
    bear::universe::position_type m_anchor_movement;

    /** \brief Coordinate used as a reference on y-coordinate. */
    bear::universe::coordinate_type m_y_reference;

    /** \brief Gap of the last teleportation. */
    bear::universe::position_type m_teleportation_gap;
    
    /** \brief The current object to drop. */
    handle_type m_current_drop_item;

    /** \brief The dropped objects. */
    std::list<handle_type> m_drop_items;

    /** \brief Coordinate where item must be dropped. */
    bear::universe::position_type m_drop_position;

    /** \brief Indicates if the trap is open. */
    bool m_open;   

    /** \brief The duration since the trap is openned. */
    bear::universe::time_type m_trap_duration;

    /** \brief X-coordinate gap of anchor in the last interation. */
    bear::universe::coordinate_type m_last_anchor_gap_x;

    /** \brief Indicates that boss move on the cart. */
    bool m_move_on_cart;

    /** \brief The item for interactive item. */
    bear::reference_item* m_interactive_item;

    /** \brief The decorative module to put in transition_level. */
    bear::engine::base_item* m_module;  

    /** \brief The module serial to put in transition_level. */
    unsigned int m_module_serial;

    /** \brief The minimum distance on x-coordinate between cart and anchor. */
    static bear::universe::coordinate_type s_min_x_cart_distance;

    /** \brief The maximum distance on x-coordinate between cart and anchor. */
    static bear::universe::coordinate_type s_max_x_cart_distance;

    /** \brief The minimum distance on y-coordinate between cart and anchor. */
    static const bear::universe::coordinate_type s_min_y_cart_distance;

    /** \brief The maximum distance on y-coordinate between cart and anchor. */
    static const bear::universe::coordinate_type s_max_y_cart_distance;

  }; // class boss
} // namespace rp

#endif // __RP_BOSS_HPP__
