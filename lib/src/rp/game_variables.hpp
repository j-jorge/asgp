/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class that helps accessing to the game variables.
 * \author Julien Jorge
 */
#ifndef __RP_GAME_VARIABLES_HPP__
#define __RP_GAME_VARIABLES_HPP__

#include <string>

namespace rp
{
  /**
   * \brief The class that helps accessing to the game variables.
   * \author Julien Jorge
   */
  class game_variables
  {
  public:
    // current level number
    static unsigned int get_level_number();
    static void set_level_number( unsigned int n );
 
    // current serial number
    static unsigned int get_serial_number();
    static void set_serial_number( unsigned int n );

    // current unlocked serial
    static unsigned int get_unlocked_serial();
    static void set_unlocked_serial( unsigned int n );

    // last serial
    static unsigned int get_last_serial();
    static void set_last_serial(unsigned value);

    // selected serial
    static unsigned int get_selected_serial();
    static void set_selected_serial(unsigned value);

    // level info
    static std::string get_level_info();
    static void set_level_info( const std::string& n );

    // level name
    static std::string get_level_name();
    static void set_level_name( const std::string& n );

    // Theme of level
    static std::string get_level_theme();
    static void set_level_theme( const std::string& t );

    // level selection
    static bool selected_level_exist();
    static void select_level(bool value);

    // go order
    static bool get_go_order_status();
    static void set_go_order_status(bool value);

    // back order
    static bool get_back_order_status();
    static void set_back_order_status(bool value);

    // movement order
    static bool get_movement_order_status();
    static void set_movement_order_status(bool value);

    // loading status
    static bool get_in_loading();
    static void set_in_loading(bool value);

    // boss level
    static bool is_boss_level();
    static void set_boss_level(bool value);
    static bool is_boss_transition();
    static void set_boss_transition(bool value);

    // demo version
    static bool is_demo_version();
    static void set_demo_version(bool value);

    // level ending
    static bool is_level_ending();
    static void set_level_ending(bool value);

    // ending effect
    static bool get_ending_effect();
    static void set_ending_effect(bool value);

    // level starting
    static bool level_has_started();
    static void set_level_starting(bool value);

    // cart elements
    static std::string get_cart_elements_number_variable_name(unsigned int id);
    static unsigned int get_cart_elements_number(unsigned int id);
    static void set_cart_elements_number(unsigned int id, unsigned int value);

    // plunger
    static std::string get_plunger_total_number_variable_name();
    static unsigned int get_plunger_total_number();
    static void set_plunger_total_number(unsigned int value);
    static std::string get_plunger_number_variable_name();
    static unsigned int get_plunger_number();
    static void set_plunger_number(unsigned int value);
    static std::string get_plunger_activation_variable_name();
    static bool get_plunger_activation();
    static void set_plunger_activation(bool value);
    static std::string get_plunger_validity_variable_name();
    static bool get_plunger_validity();
    static void set_plunger_validity(bool value);

    // cannonball
    static std::string get_cannonball_activation_variable_name();
    static bool get_cannonball_activation();
    static void set_cannonball_activation(bool value);
    static std::string get_cannonball_validity_variable_name();
    static bool get_cannonball_validity();
    static void set_cannonball_validity(bool value);
    
    // status visibility
    static std::string get_status_visibility_variable_name();
    static bool get_status_visibility();
    static void set_status_visibility(bool value);
    
    // score
    static std::string get_persistent_score_variable_name
    (unsigned int serial, unsigned int number);
    static std::string get_score_variable_name();
    static unsigned int get_persistent_score();
    static unsigned int get_persistent_score
    (unsigned int serial, unsigned int number);
    static void set_persistent_score( unsigned int c );
    static unsigned int get_score();
    static void set_score( unsigned int c );
    static void add_score
    ( unsigned int combo, int points, bool floating_score );

    //combo
    static std::string get_last_combo_variable_name();
    static unsigned int get_last_combo();
    static void set_last_combo( unsigned int combo );

    // level_state
    static std::string get_level_state_variable_name
    (unsigned int serial, unsigned int number);
    static unsigned int get_level_state
    ( unsigned int serial, unsigned int number );
    static void set_level_state
    ( unsigned int serial, unsigned int number, unsigned int level_state );
    
    // medals
    static unsigned int get_last_medal();
    static void set_last_medal(unsigned value);
    static unsigned int get_bronze_threshold();
    static void set_bronze_threshold( unsigned int threshold );
    static unsigned int get_silver_threshold();
    static void set_silver_threshold( unsigned int threshold );
    static unsigned int get_gold_threshold();
    static void set_gold_threshold( unsigned int threshold );

    // floating_score
    static std::string get_new_score_variable_name();
    static void set_new_score(bool value);
    static std::string get_combo_variable_name();
    static unsigned int get_combo();
    static void set_combo(unsigned int c );
    static std::string get_points_variable_name();
    static int get_points();
    static void set_points(int c );
    static double get_score_rate_x();
    static void set_score_rate_x(double x);
    static double get_score_rate_y();
    static void set_score_rate_y(double y);

    // boss
    static std::string get_boss_hits_variable_name();
    static unsigned int get_boss_hits();
    static void set_boss_hits( unsigned int nb );

    // balloons
    static std::string get_persistent_balloon_variable_name
    (unsigned int serial, unsigned int number);
    static unsigned int get_persistent_balloon();
    static unsigned int get_persistent_balloon
    (unsigned int serial, unsigned int number);
    static void set_persistent_balloon( unsigned int b );

    static std::string get_balloon_variable_name();
    static unsigned int get_balloons_number();
    static void set_balloons_number( unsigned int nb );
    static unsigned int get_required_balloons_number();
    static void set_required_balloons_number( unsigned int nb );

    // bad balloon
    static std::string get_bad_balloon_variable_name();
    static unsigned int get_bad_balloon_number();
    static void set_bad_balloon_number( unsigned int nb );

    // bad plunger
    static std::string get_bad_plunger_variable_name();
    static unsigned int get_bad_plunger_number();
    static void set_bad_plunger_number( unsigned int nb );

    // bad cannonball
    static std::string get_bad_cannonball_variable_name();
    static unsigned int get_bad_cannonball_number();
    static void set_bad_cannonball_number( unsigned int nb );    
    
    // balloons intensity
    static double get_balloon_red_intensity();
    static void set_balloon_red_intensity( double red );
    static double get_balloon_green_intensity();
    static void set_balloon_green_intensity( double green );
    static double get_balloon_blue_intensity();
    static void set_balloon_blue_intensity( double blue );
    
    // persistent utilities
    static std::string make_persistent_variable_name( const std::string& n );
    static void load_variables(const std::string& lvl);
    static void save_variables(const std::string& lvl);
  }; // class game_variables
} // namespace rp

#endif // __RP_GAME_VARIABLES_HPP__
