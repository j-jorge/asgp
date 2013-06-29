/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a cart.
 * \author Sebastien Angibaud
 */
#ifndef __RP_CART_HPP__
#define __RP_CART_HPP__

#include "rp/plunger.hpp"
#include "rp/item_that_speaks.hpp"

#include "audio/sound_manager.hpp"

#include "engine/item_brick/item_with_input_listener.hpp"
#include "engine/item_brick/item_with_toggle.hpp"
#include "engine/model.hpp"
#include "engine/base_item.hpp"
#include "engine/export.hpp"
#include "visual/animation.hpp"

#include "generic_items/decorative_item.hpp"
#include "generic_items/link/continuous_link_visual.hpp"
#include "generic_items/rocket.hpp"
#include "generic_items/timer.hpp"

#include <claw/tween/single_tweener.hpp>
#include <boost/signal.hpp>
#include <set>

namespace rp
{
  class attractable_item;
  class cursor;
  class decorative_balloon;
  
  /**
   * \brief The class describing a cart.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class cart :
    public bear::engine::model
  < bear::engine::item_with_input_listener
    < bear::engine::item_with_toggle
      < item_that_speaks
        < bear::engine::base_item > > > >
  {
    DECLARE_BASE_ITEM(cart);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model
    < bear::engine::item_with_input_listener
      < bear::engine::item_with_toggle
      < item_that_speaks
        < bear::engine::base_item > > > > super;

    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
  private:
    typedef void (cart::*progress_function_type)(bear::universe::time_type);
    typedef std::set<plunger*> plungers_set;

  public:
    cart();
    cart( const cart& that );
    ~cart();

    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void get_visual( std::list<bear::engine::scene_visual>& visuals ) const;
    bool set_bool_field( const std::string& name, bool value );
    bool set_u_integer_field( const std::string& name, unsigned int value );
    bool set_item_field
    ( const std::string& name, bear::engine::base_item* value );

    void create_hit_star( bear::engine::base_item& ref ) const;
    void create_cannon_fire();
    void regenerate();
    void finish_plunger(plunger* p, plunger::handle_type& attracted_item);
    void is_hit();
    void die_by_wall();
    void die(bool right, bool left);
    void jump();

    // Accessors
    void set_right_orientation(bool right_orientation);
    void set_ground_force(const bear::universe::force_type& ground_force);
    void set_force_factor(const bear::universe::coordinate_type& force_factor);
    void set_can_jump(bool can_jump);
    void set_looping(bool looping);
    void apply_takeoff();
    unsigned int get_score() const;

    void set_passive();

    void save_position();
    bear::universe::position_type get_plunger_position() const;
    bear::universe::position_type 
    get_balloon_anchor_position(const std::string& anchor_name ) const;
    bool can_finish() const;

    void give_element();
    void give_plunger_bonus();
    void set_level_timer(const bear::timer* level_timer);
    const bear::timer* get_level_timer() const;
    void create_level_ending_effect();

  private:
    void get_dependent_items( std::list<physical_item*>& d ) const;
    void add_balloon(attractable_item* attracted_item);
    bool check_death();
    void lose_elements();
    void lose_balloons(unsigned int nb);
    void add_plunger_link
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void add_bad_plunger_zone
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void remove_elements();
    void break_element
    (const std::string& name, const bear::universe::force_type& force);

    // Spot 
    bear::universe::position_type hot_spot() const;
    void set_spot_target_position
    ( bear::universe::coordinate_type x, bear::universe::coordinate_type y );
    void set_spot_balance_move
    ( bear::universe::coordinate_type x, bear::universe::coordinate_type y );
    void add_spot_gap( const bear::universe::position_type& gap );
    void balance_spot( bool x, bool y );
    void progress_spot( bear::universe::time_type elapsed_time );
    
    // Other
    void create_cursor();
    decorative_balloon* create_decorative_balloon
    (attractable_item* attracted_item, const std::string& anchor_name);
    void create_link_on_balloon
    (decorative_balloon* balloon, const std::string& anchor_name,
     unsigned int index);
    void create_disappear_effect(decorative_balloon* balloon);
    bool can_throw_cannonball();
    bool can_throw_plunger();
    void throw_cannonball();
    void throw_plunger();
    void init_elements();
    void init_element(const std::string& name);
    void clear_balloons();
    
    // Update fonction
    void give_force_movement();
    void give_sky_force_movement();
    void give_looping_force_movement();
    void progress_tweeners(bear::universe::time_type elapsed_time);
    void progress_injured_state(bear::universe::time_type elapsed_time);
    void progress_arm_angle();
    void progress_plunger();
    void progress_cannon();
    void progress_fire();

    // progress
    void progress_idle( bear::universe::time_type elapsed_time );
    void progress_move( bear::universe::time_type elapsed_time );
    void progress_jump( bear::universe::time_type elapsed_time );
    void progress_fall( bear::universe::time_type elapsed_time );
    void progress_crouch( bear::universe::time_type elapsed_time );
    void progress_dead( bear::universe::time_type elapsed_time );
    void progress_takeoff( bear::universe::time_type elapsed_time );
    void progress_with_tar( bear::universe::time_type elapsed_time );
    void check_crouch();

    // start functions
    void start_idle();
    void start_move();
    void start_jump();
    void start_fall();
    void start_crouch();
    void start_dead();
    void start_with_tar();
    void start_takeoff();
    
    // apply fonction
    void apply_impulse_jump();  
    void give_impulse_force();  
    void apply_crouch();
    void apply_stop_crouch();
    
    // controls
    bool key_pressed( const bear::input::key_info& key );
    bool key_released( const bear::input::key_info& key );
    bool mouse_released( bear::input::mouse::mouse_code button,
			 const claw::math::coordinate_2d<unsigned int>& pos );
    bool mouse_move( const claw::math::coordinate_2d<unsigned int>& pos );
    
    // evenement
    void on_toggle_on( bear::engine::base_item* activator );
    void on_painter_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );
    void on_fire_angle_change(const bear::universe::coordinate_type& value);
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

    // other
    void create_wave( bool double_wave );
    void create_smoke( double elapsed_time );
    void update_bottom_contact();
    void update_status_informations();
    bool test_in_sky();
    bear::universe::position_type compute_gun_position() const;
    bear::universe::position_type compute_fire_position() const;

    std::string get_theme_image_name() const;
    bool collision_with_tar( bear::engine::base_item& that );
    bool collision_with_bonus( bear::engine::base_item& that );
    bool collision_with_explosion
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    bool collision_with_cannonball( bear::engine::base_item& that );

    void create_balloons( std::size_t count ) const;

    void init_injured_state();
    void check_takeoff();

    void create_rocket() const;
    bear::rocket* create_small_rocket() const;
    bear::visual::color get_random_rocket_color() const;
    std::string get_rocket_explosion_sound_name() const;
    void init_signals();
    void on_combo_changed(unsigned int combo);
    std::string get_combo_sample_name( unsigned int combo ) const;

  private:
    static void init_exported_methods();

  private:
    /** \brief Current progress function. */
    progress_function_type m_progress;
    
    /** \brief Indicates if cart has right orientation. */
    bool m_right_orientation;
    
    /** \brief Force to give at tje cart on the ground. */
    bear::universe::force_type m_ground_force;
    
    /** \brief Position of the hot spot, relative to the center of mass. */
    bear::universe::position_type m_hot_spot_position;
    
    /** \brief The best position of the hot spot. */
    bear::universe::position_type m_hot_spot_target;
    
    /** \brief Maximum movement allowed when balancing the hot spot. */
    bear::universe::position_type m_hot_spot_balance_move;
    
    /** \brief The item center on hot spot. */
    bear::engine::base_item* m_spot_item;

    /** \brief The item for plunger sight. */
    cursor* m_cursor;

     /** \brief Indicates that cart is on a switching. */
    bool m_on_switching;

    /** \brief The tweener for change angle of fire. */
    claw::tween::single_tweener m_tweener_fire_angle;

    /** \brief Tell if the cart had a bottom contact at the previous
        progress(). */
    bool m_previous_bottom_contact;

    /** \brief The speed of the cart at the last collision. */
    bear::universe::speed_type m_speed_on_contact;

    /** \brief Tell if the cart had a top contact at the previous
        progress(). */
    bool m_previous_top_contact;
    
    /** \brief The mouse position. */
    bear::universe::position_type m_gap_mouse;

    /** \brief The factor applied on force in move state. */
    bear::universe::coordinate_type m_force_factor;

    /** \brief The origin position. */
    bear::universe::position_type m_origin;

    /** \brief The origin z-position. */
    int m_origin_z;

    /** Indicates that we want crouch. */
    bool m_want_crouch;

    /** Indicates if cart can jump. */
    bool m_can_jump;

    /** Indicates if cart is in a top looping. */
    bool m_looping;

    /** A set of pointer on plungers. */
    plungers_set m_plungers;

    /** The maximum number of plungers. */
    unsigned int m_maximum_plungers;

    /** Indicates for each element if there are intact. */
    std::map<std::string, bool> m_elements;

    /** The list of balloons. */
    std::list<decorative_balloon*> m_balloons;

    /** \brief The decoration of the link on balloon. */
    bear::continuous_link_visual* m_link_balloon_visual;

    /** \brief The angle of the arm in each action. */
    double m_arm_angle;

    /** \brief Indicates if canon has a good angle. */
    bool m_good_fire;

    /** \brief Indicates if arm has a good angle. */
    bool m_good_arm_angle;

    /** \brief Indicates if the cart is injured. */
    bool m_is_injured;

    /** \brief A pointer on level timer. */
    const bear::timer* m_level_timer;

    /** \brief Duration since the cart is injured. */
    bear::universe::time_type m_injured_duration;
    
    /** \brief Duration since the cart has fired. */
    bear::universe::time_type m_fire_duration;

    /** \brief How long we are in the takeoff action. */
    bear::universe::time_type m_takeoff_duration;

    /** \brief Tells how long to wait before creating a new smoke cloud. */
    bear::universe::time_type m_next_smoke;

    /** \brief Tells that the cart in not controlled by the player. */
    bool m_passive;
    
    /** \brief The identifier of the cart. */
    unsigned int m_id;

    /** \brief Indicates if the bad plunger zone is renedered. */
    bool m_bad_plunger_zone_rendering;

    /** \brief The connections to various signals. */
    std::list<boost::signals::connection> m_signals;

    /** \brief The sample of combo. */
    bear::audio::sample* m_combo_sample;

    /** \brief The score. */
    static unsigned int s_score;

    /** \brief Duration in which the cart is injured. */
    static const bear::universe::time_type s_injured_duration;

    /** \brief Duration in which the cart cannot fire. */
    static const bear::universe::time_type s_fire_duration;

    /** \brief The delay between two smoke clouds. */
    static const bear::universe::time_type s_smoke_delay;

    /** \brief The maximum cannon angle. */
    static const double s_max_cannon_angle;

    /** \brief The minimum cannon angle. */
    static const double s_min_cannon_angle;

    /** \brief The maximum arm angle. */
    static const double s_max_arm_angle;

    /** \brief The minimum arm angle. */
    static const double s_min_arm_angle;

    /** \brief The minimum speed length. */
    static const double s_min_speed_length;

    /** \brief Number of decorative ballons. */
    static const unsigned int s_decorative_balloons_number;

  }; // class cart
} // namespace rp

#endif // __RP_CART_HPP__
