/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a item for level selection.
 * \author Sebastien Angibaud
 */
#ifndef __RP_LEVEL_SELECTOR_HPP__
#define __RP_LEVEL_SELECTOR_HPP__

#include "rp/entity.hpp"

#include "engine/base_item.hpp"
#include "engine/export.hpp"
#include "engine/item_brick/item_with_input_listener.hpp"
#include "generic_items/cursor.hpp"
#include "generic_items/decorative_rectangle.hpp"
#include "generic_items/decorative_item.hpp"
#include "universe/derived_item_handle.hpp"
#include "visual/writing.hpp"
#include "visual/animation.hpp"

#include <claw/tween/tweener_group.hpp>
#include <claw/tween/single_tweener.hpp>

#include <boost/signals2/connection.hpp>

namespace rp
{
  class cart;

  /**
   * \brief The class describing a item for level selection.
   *
   * The valid fields for this item are
   *  \c (unsigned int) serial_number The number of the level serial (required).
   *  \c (unsigned int) level_number The number of the level (required).
   *  \c (unsigned int) required_balloons Indicates the number of required 
   *     balloons.
   *  \c (unsigned int) required_medal Indicates the required medals to active
   * the level.
   *  \c (unsigned int) bronze_threshold The threshold to give the bronze medal.
   *  \c (unsigned int) silver_threshold The threshold to give the silver medal.
   *  \c (unsigned int) gold_threshold The threshold to give the gold medal.
   *  \c (list of items) precedence The level selectors that preceeds 
   * the current level selector.
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class level_selector :
    public bear::engine::item_with_input_listener
  < bear::engine::base_item >,
    public entity
  {
    DECLARE_BASE_ITEM(level_selector);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::item_with_input_listener
    < bear::engine::base_item > super;
  
  private:
    /** \brief The type of an handle on a level selector. */
    typedef bear::universe::derived_item_handle<level_selector> handle_type;

    /** \brief The type of a list of level selectors. */
    typedef std::list<handle_type> handle_list;

  public:
    level_selector();
    level_selector( const level_selector& that );
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    bool set_item_field
    ( const std::string& name, bear::engine::base_item* value );
    bool set_item_list_field
    ( const std::string& name, 
      const std::vector<bear::engine::base_item*>& value );
    bool set_u_integer_field
    ( const std::string& name, unsigned int value );
    bool set_string_field
    ( const std::string& name, const std::string& value );
    void get_visual( std::list<bear::engine::scene_visual>& visuals ) const;

  private:
    void activate();

    bool finger_action( const bear::input::finger_event& event );
    bool mouse_released( bear::input::mouse::mouse_code button,
			 const claw::math::coordinate_2d<unsigned int>& pos );
    bool mouse_move( const claw::math::coordinate_2d<unsigned int>& pos );

    void load_level();
    void update_visibility();
    void update_state();
    void update_score();
    bool check_precedence() const;
    unsigned int get_state() const;
    unsigned int get_new_state() const;

    void render_background
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void render_hidden_sprite
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void render_border
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void render_level
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void render_star
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void render_points
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void render_medal
    ( std::list<bear::engine::scene_visual>& visuals ) const;

    void start_update();
    void end_update();
    void unlock();
    void show_star();
    void show_medal(unsigned int medal);

    void on_unlock_change();
    void on_star_change();
    void on_gap_y_change(bear::universe::coordinate_type gap_y);
    void on_unlock_angle_change( double angle );
    void on_move_on_center( double factor );
    void on_unlock_factor_change( double factor );
    void push_level();
    void on_star_angle_change( double angle );
    void on_medal_factor_change( double factor );
    void on_medal_y_gap_change( double gap_y );
    void update_medal(unsigned int medal);

    bool is_boss_level() const;
    bool is_selected_level() const;
    void init_selected_level();
    void select_level();
    void move_on_center();
    void come_back();
    void start_move_back();
    void move_back();
    void create_opaque_rectangle();
    void create_level_name();
    void remove_opaque_rectangle();
    void kill_rectangle();
    void kill_level_name();
    void remove_level_name();
    void check_orders();
    void check_back_order();
    void check_go_order();
    void check_level_ending();
    void resume();
    bool check_fall_medal();
    void stop_selection();
    bear::visual::position_type get_medal_ending_position() const;
    bear::visual::position_type get_medal_position() const;
    void choose_level_sprite();

  private:
    /** \brief Indicates that the state changes. */
    bool m_updated;

    /** \brief The serial number. */
    unsigned int m_serial_number;

    /** \brief The number of the level. */
    unsigned int m_level_number;
    
    /** \brief Indicates the required medal to active the level. */
    unsigned int m_required_medal;

    /** \brief The threshold for bronze medal. */
    unsigned int m_bronze_threshold;      

    /** \brief The threshold for silver medal. */
    unsigned int m_silver_threshold;

    /** \brief The threshold for gold medal. */
    unsigned int m_gold_threshold;

    /** \brief The number of required balloons. */
    unsigned int m_required_balloons;

    /** \brief The initial factor of level sprite. */
    double m_init_level_factor; 

    /** \brief Indicates if the selector is visible. */
    unsigned int m_visible;

    /** \brief The level selectors that preceeds the current level selector. */
    handle_list m_precedence;

    /** \brief Thenn font for text. */
    bear::visual::font m_font;
    
    /** \brief The points text. */
    bear::visual::writing m_points;

    /** \brief The star. */
    bear::visual::animation m_star;

    /** \brief The gap of star sprite. */
    bear::universe::position_type m_star_gap;

    /** \brief The sprite of level. */
    bear::visual::sprite m_level_sprite;  

    /** \brief The sprite in hidden state. */
    bear::visual::sprite m_hidden_sprite; 

    /** \brief The sprite of background. */
    bear::visual::sprite m_background_sprite;  

    /** \brief The gap of background sprite. */
    bear::universe::position_type m_background_gap;

    /** \brief The sprite of border. */
    bear::visual::sprite m_border_sprite;  

    /** \brief The sprite of medal. */
    bear::visual::sprite m_medal_sprite;    

    /** \brief The set of current tweeners. */
    claw::tween::tweener_group m_tweeners;

    /** \brief The factor of level sprite. */
    double m_level_factor;

    /** \brief The factor of medal sprite. */
    double m_medal_factor;    

    /** \brief The factor of score text. */
    double m_score_factor;

    /** \brief The theme of the level. */
    std::string m_theme;

    /** \brief The gap on y-coordinate of the level name. */
    bear::universe::coordinate_type m_level_name_gap_y;

    /** \brief The name of the level. */
    std::string m_level_name;

    /** \brief The gap of medal sprite. */
    bear::universe::position_type m_medal_gap;

    /** \brief Indicates if the mouse is in the item at the last iteration. */
    bool m_mouse_in;

    /** \brief The initial center of mass. */
    bear::universe::position_type m_initial_position;

    /** \brief A pointer on the decorative rectangle. */
    bear::decorative_rectangle * m_rectangle;   

    /** \brief A pointer on the decorative item for level name. */
    bear::decorative_item * m_decorative_level_name;   

    /** \brief Indicates if a level is in loading. */
    bool m_load;

    /** \brief The opacity of the rectangle. */
    double m_rectangle_opacity;    

    /** \brief The factor of medal movement. */
    double m_medal_movement;   

    /** \brief The tweener for rectangle opacity. */
    claw::tween::single_tweener m_tweener_rectangle_opacity;

    /** \brief The tweener for medal movement. */
    claw::tween::single_tweener m_tweener_medal;

    /** \brief The tweener for star rotation. */
    claw::tween::single_tweener m_tweener_star;

    /** \brief A pointer on the cursor. */
    bear::cursor* m_cursor;

    /** \brief A state of the level. */
    unsigned int m_level_state;

    boost::signals2::scoped_connection m_ad_connection;
    
    /** \brief Indicates if a level is selected. */
    static bool s_selection;

  }; // class level_selector
} // namespace rp

#endif // __RP_LEVEL_SELECTOR_HPP__
