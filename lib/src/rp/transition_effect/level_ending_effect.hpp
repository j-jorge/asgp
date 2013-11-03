/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The effect displayed at the ending of the levels.
 * \author Julien Jorge
 */
#ifndef __RP_LEVEL_ENDING_EFFECT_HPP__
#define __RP_LEVEL_ENDING_EFFECT_HPP__

#include "audio/sound_manager.hpp"

#include "engine/transition_effect/transition_effect.hpp"
#include "engine/world.hpp"
#include "generic_items/decorative_item.hpp"

#include "gui/visual_component.hpp"
#include "gui/button.hpp"

#include "visual/writing.hpp"

#include <claw/tween/single_tweener.hpp>
#include <claw/tween/tweener_group.hpp>

#include <boost/thread/mutex.hpp>

namespace rp
{
  class cart;

  /**
   * \brief The effect displayed at the ending of the levels.
   * \author Julien Jorge
   */
  class level_ending_effect:
    public bear::engine::transition_effect
  {
  public:
    /** \brief The type of a list of scene elements retrieved from the layer.*/
    typedef std::list<bear::visual::scene_element> scene_element_list;

  private:
    /**
     * \brief The score_request class is passed to the thread that send
     *        requests to the score server in order to asynchronously retrieve
     *        the scores.
     *
     * If the owner of the score_request does not need the score, it can call
     * the disable() method to cancel the request. All copies of the instances
     * will then be disabled.
     *
     * \author Julien Jorge
     */
    class score_request
    {
    public:
      /** \brief The type of the function to call when the score is ready. */
      typedef boost::function<void (unsigned int)> callback_type;

    public:
      score_request();
      score_request( callback_type c, std::string level_name );
      score_request( const score_request& that );

      std::string get_level_name() const;
      void disable();

      void operator()();
      score_request& operator=( score_request that );

    private:
      void call_callback( unsigned int c );

    private:
      /** \brief The function to call when the score is set. */
      callback_type m_callback;

      /** \brief The file name of the level for which we want the score. */
      std::string m_level_name;

      /** \brief Tells if the score must be passed to the callback. */
      boost::shared_ptr<bool> m_is_active;

      /** \brief The mutex used to restrict the access to m_is_active. */
      boost::shared_ptr<boost::mutex> m_shared_mutex;

    }; // class score_request

    /** \brief A line of points displayed on the screen. */
    class score_line
    {
    public:
      score_line
      ( const bear::visual::font& f, const std::string& text, int points,
        const bear::visual::sprite& picture );

      score_line
      ( const bear::visual::font& f,
        const std::string& text, const std::string& coeff, 
        const std::string& count, int points,
        const bear::visual::sprite& picture );

      void render
      ( scene_element_list& e, bear::visual::coordinate_type left,
        bear::visual::coordinate_type right ) const;

      int update_score( int delta );
      int get_total_points() const;

      bear::visual::coordinate_type get_height() const;

      double get_y_position() const;
      void set_y_position( double y );

      void set_negative();
      bool is_negative() const;

    private:
      /** \brief The font used for the writings. */
      bear::visual::font m_font;

      /** \brief Explain the reason of the points. */
      bear::visual::writing m_label;

      /** \brief The points computation. */
      bear::visual::writing m_computation_label;      

      /** \brief The points computation coefficient. */
      bear::visual::writing m_computation_coefficient_label;

      /** \brief The remaining points, as a text. */
      bear::visual::writing m_points_text;

      /** \brief The sprite of the bonus picture. */
      bear::visual::sprite m_bonus_sprite;

      /** \brief The total number of points. */
      const int m_total_points;

      /** \brief The current number of points. */
      int m_current_points;

      /** \brief The y-position of the score. */
      bear::visual::coordinate_type m_y;

      /** \brief Tells that this line is a negative score. */
      bool m_negative;

      /** \brief A delta applied to the text to create shadows. */
      static const bear::visual::coordinate_type s_shadow_delta;

      /** \brief Margin on the right of bonus picture. */
      static const bear::visual::coordinate_type s_bonus_picture_margin;

      /** \brief The scale factor applied to the text. */
      static const double s_scale_factor;

    }; // class score_line

    /** \brief The type of the function to call to update the score lines. */
    typedef void (rp::level_ending_effect::*update_function_type)
    ( bear::universe::time_type );

  public:
    level_ending_effect();
    level_ending_effect( const level_ending_effect& that );
    ~level_ending_effect();

    void set_world( const bear::engine::world* w );

    bool is_finished() const;
    void build();
    bear::universe::time_type
    progress( bear::universe::time_type elapsed_time );
    void render( scene_element_list& e ) const;
    void set_cart(const cart* c);

  private:
    void create_controls();
    void fill_controls();

    bool key_pressed( const bear::input::key_info& key );
    bool key_maintained( const bear::input::key_info& key );
    bool button_pressed
    ( bear::input::joystick::joy_code button, unsigned int joy_index );
    bool button_maintained
    ( bear::input::joystick::joy_code button, unsigned int joy_index );
    bool mouse_move( const claw::math::coordinate_2d<unsigned int>& pos );
    bool mouse_pressed
    ( bear::input::mouse::mouse_code button,
      const claw::math::coordinate_2d<unsigned int>& pos );
    bool finger_action( const bear::input::finger_event& event );

    bear::visual::position_type get_event_position
    ( const claw::math::coordinate_2d<unsigned int>& pos ) const;

    void skip();
    void fill_points();

    void give_level_points( const bear::visual::font& f );
    void give_balloon_points(const bear::visual::font& f);
    void give_bad_balloon_points(const bear::visual::font& f);
    void give_bad_plunger_points(const bear::visual::font& f);
    void give_bad_cannonball_points(const bear::visual::font& f);
    void give_cart_elements_points(const bear::visual::font& f);
    void give_time_points(const bear::visual::font& f);

    bear::visual::coordinate_type get_lines_top() const;
    bear::visual::coordinate_type get_score_top() const;

    void initialize_line_position( std::list<score_line>& lines ) const;

    bool update_lines( bear::universe::time_type elapsed_time );

    void update_positive_lines( bear::universe::time_type elapsed_time );
    void update_negative_lines( bear::universe::time_type elapsed_time );

    void delay_positive_merge( bear::universe::time_type elapsed_time );
    void delay_negative_merge( bear::universe::time_type elapsed_time );

    void merge_positive_lines( bear::universe::time_type elapsed_time );
    void merge_negative_lines( bear::universe::time_type elapsed_time );

    void flash_positive_persistent( bear::universe::time_type elapsed_time );
    void flash_negative_persistent( bear::universe::time_type elapsed_time );

    bool update_lines
    ( bear::universe::time_type elapsed_time,
      std::list<score_line>& lines ) const;
    bool merge_lines
    ( bear::universe::time_type elapsed_time,
      std::list<score_line>& lines ) const;

    void create_persistent_line
    ( std::string label, std::list<score_line>& lines, std::string icon_name );

    void update_tick( bear::universe::time_type elapsed_time );
    void update_medal();
    void update_unlocked_serial() const;
    void create_decorative_medal();
    unsigned int get_medal() const;
    void render_score( scene_element_list& e ) const;
    void render_score_lines
    ( scene_element_list& e, const std::list<score_line>& lines ) const;
    void render_score_background( scene_element_list& e ) const;
    void render_flash_line( scene_element_list& e) const;
    void render_level_name( scene_element_list& e) const;
    void render_opaque_rectangle( scene_element_list& e) const;
    void render_medal( scene_element_list& e) const;
    void render_background( scene_element_list& e ) const;
    void create_fade_out_tweener();
    void pop_level();
    void add_button_component();

    void add_social_buttons();

    void add_facebook_button();
    void create_facebook_tweener();

    void add_twitter_button();
    void create_twitter_tweener();

    void get_best_score();
    void set_best_score( unsigned int );

    void on_pass_scores();
    void on_facebook_click();
    void on_twitter_click();

    // not implemented
    level_ending_effect& operator=( const level_ending_effect& that );

  private:
    /** \brief The lines that do not disappear. */
    std::list<score_line> m_persistent_points;

    /** \brief The remaining positive points to give. */
    std::list<score_line> m_positive_points;

    /** \brief The remaining negative points to give. */
    std::list<score_line> m_negative_points;

    /** \brief The points, as a text. */
    bear::visual::writing m_points_text;

    /** \brief The best score for the level, worldwide. */
    bear::visual::writing m_world_record;

    /** \brief The sprite of the medal picture. */
    bear::visual::sprite m_medal_sprite;

    /** \brief The sprite of the gold medal picture. */
    bear::visual::sprite m_gold_medal_sprite;
    
    /** \brief The sprite of the silver medal picture. */
    bear::visual::sprite m_silver_medal_sprite;

    /** \brief The sprite of the bronze medal picture. */
    bear::visual::sprite m_bronze_medal_sprite;

    /** \brief A factor applied to s_points_per_second. */
    double m_speed_factor;

    /** \brief The date at which we emit the next tick sound. */
    bear::universe::time_type m_next_tick;

    /** \brief The world where we search for the bonus points. */
    const bear::engine::world* m_world;

    /** \brief A pointer on the cart. */
    const cart* m_cart;

    /** \brief Indicates that the effect is finished. */
    bool m_finished;

    /** \brief Indicates the medal state. */
    unsigned int m_medal;

    /** \brief The name of the level. */
    bear::visual::writing m_level_name;

    /** \brief The opacity of the fade out. */
    double m_rectangle_opacity;

    /** \brief The tweener for fade_out opacity. */
    claw::tween::single_tweener m_tweener_fade_out;

    /** \brief The tweener for the social buttons. */
    claw::tween::tweener_group m_social_tweener;

    /** \brief The current decorative medal. */
    bear::decorative_item* m_decorative_medal;

    /** \brief Indicates that the level has been popped. */
    bool m_pop_level;
    
    /** \brief The window containing the components of the layer. */
    bear::gui::visual_component m_root_window;

    /** \brief Indicates that component is highlighted. */
    bool m_active_component;

    /** \brief The button. */ 
    bear::gui::button* m_skip_button;

    /** \brief The Facebook button. */
    bear::gui::button* m_facebook_button;

    /** \brief The Twitter button. */
    bear::gui::button* m_twitter_button;

    /** \brief The sprite of background when the mouse is on the button. */
    bear::visual::sprite m_background_on_sprite;  

    /** \brief The sprite of background when the mouse is out the button. */
    bear::visual::sprite m_background_off_sprite;  

    /** \brief Indicates that the fade out has been started. */
    bool m_in_fade_out;

    /** \brief The sprite of the new record. */
    bear::visual::sprite m_new_record_sprite;

    /** \brief The sample of applause. */
    bear::audio::sample* m_applause_sample;

    /** \brief The function to call to update the lines of score. */
    update_function_type m_update_function;

    /** \brief The current opacity of the flash effect applied on the last line
        of the persistent points. */
    double m_flash_opacity;

    /** \brief Tells if the tick sound must be played. */
    bool m_play_tick;

    /** \brief The delay to wait before merging the lines. */
    bear::universe::time_type m_merge_delay;

    /** \brief The function object that request the best score of the current
        level to our stats server. */
    score_request m_score_request;

    /** \brief How many points are given per second. */
    static const unsigned int s_points_per_second;

    /** \brief The default margin around the score lines. */
    static const bear::visual::coordinate_type s_screen_margin;

    /** \brief The margin between two score lines. */
    static const bear::visual::coordinate_type s_margin;

    /** \brief The speed in screen unit/sec. of the movement of a score line. */
    static const double s_score_line_speed;

  }; // class level_ending_effect
} // namespace rp

#endif // __RP_LEVEL_ENDING_EFFECT_HPP__
