/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Utility functions about roller painting.
 * \author SÃ©bastien angibaud
 */
#ifndef __RP_UTIL_HPP__
#define __RP_UTIL_HPP__

#include "engine/base_item.hpp"

namespace rp
{
  /**
   * \brief Utility functions about roller painting.
   * \author Sébastien angibaud
   */
  class util
  {
  public:
    static bear::universe::position_type get_level_name_position
    ( const bear::universe::rectangle_type & camera_rect);
    static bear::universe::position_type get_medal_position
    ( const bear::universe::rectangle_type & camera_rect);
    
    static std::string get_level_name();

    static void create_floating_score
    ( bear::engine::base_item& item, unsigned int points);

    static void choose_balloon_color();

    static void create_smoke
    ( const bear::engine::base_item& ref, std::size_t count,
      double min_intensity, double max_intensity, int z_shift );
    static void load_game_variables();
    static void save_game_variables();

    static void create_hit_star
    ( bear::engine::base_item& ref, bear::visual::color_type border_color,
      bear::visual::color_type fill_color,
      bear::universe::time_type time_to_live );

    static void send_quit_level( const std::string& filename );
    static void send_complete_level( const std::string& filename );
    static void send_fail_level( const std::string& filename );
    static void send_die_level( const std::string& filename );
    static void send_version();

    static void save_config();
    
  private:
    static void create_smoke
    ( const bear::engine::base_item& ref, double min_intensity,
      double max_intensity, int z_shift );

    static void apply_random_smoke_effect( bear::engine::base_item& item );

  }; // class util

} // namespace rp

#endif // __RP_UTIL_HPP__
