/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Utility functions about roller painting.
 * \author Sébastien Angibaud
 */
#include "rp/util.hpp"

#include "rp/config_file.hpp"
#include "rp/defines.hpp"
#include "rp/game_variables.hpp"
#include "rp/interactive_item.hpp"
#include "rp/entity.hpp"
#include "rp/version.hpp"

#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "engine/game.hpp"
#include "engine/system/game_filesystem.hpp"
#include "engine/variable/variable.hpp"
#include "engine/variable/variable_list_reader.hpp"
#include "engine/variable/var_map.hpp"

#include "universe/forced_movement/forced_rotation.hpp"

#include "generic_items/decorative_effect.hpp"
#include "generic_items/delayed_kill_item.hpp"
#include "generic_items/star.hpp"

#include <boost/regex.hpp>
 
/*---------------------------------------------------------------------------*/
/**
 * \brief Get the position of the level name.
 * \param camera_rect The rectangle of the camera. 
 */
bear::universe::position_type rp::util::get_level_name_position
( const bear::universe::rectangle_type & camera_rect)
{
  return 
    bear::universe::position_type
    ( camera_rect.left() + camera_rect.size().x / 2,
      camera_rect.bottom() + 0.9 * camera_rect.size().y );
} // get_level_name_position()

/*---------------------------------------------------------------------------*/
/**
 * \brief Get the position of the medal.
 * \param camera_rect The rectangle of the camera. 
 */
bear::universe::position_type rp::util::get_medal_position
( const bear::universe::rectangle_type & camera_rect)
{
  return 
#if 0
    // at right
    bear::universe::position_type
    ( camera_rect.left() + 0.85 * camera_rect.size().x,
      camera_rect.bottom() + 0.19 * camera_rect.size().y );
#else
  // at left
  bear::universe::position_type
    ( camera_rect.left() + 0.15 * camera_rect.size().x,
      camera_rect.bottom() + 0.19 * camera_rect.size().y );
#endif

} // get_medal_position()

/*---------------------------------------------------------------------------*/
/**
 * \brief Get the level_name.
 */
std::string rp::util::get_level_name()
{
  if ( game_variables::is_boss_transition() )
    return "";
  else
    {
      std::ostringstream stream;
      stream << game_variables::get_serial_number() << "-";
      
      unsigned int number = game_variables::get_level_number();
      unsigned int serial = game_variables::get_serial_number();
      
      if ( number == 5 || ( serial == 6 && number == 1 ) )
        stream << "A";
      else if ( number == 6  || ( serial == 6 && number == 2 ))
        stream << "B";
      else if ( number == 7  || ( serial == 6 && number == 3 ))
        stream << "C";
      else if ( number == 8 )
        stream << "5";
      else
        stream << number;
      
      stream << " - " << game_variables::get_level_name();
      
      return stream.str();
    }
} // util::get_level_name()

/*---------------------------------------------------------------------------*/
/**
 * \brief Create a floating score.
 * \param item The item that creates the floating score.
 * \param points Number of points to give.
 */
void rp::util::create_floating_score
( bear::engine::base_item& item, unsigned int points)
{
  game_variables::set_score_rate_x
    ( ( item.get_horizontal_middle() - item.get_level().get_camera_center().x) /
      item.get_level().get_camera()->get_width() );
  game_variables::set_score_rate_y
    ( ( item.get_vertical_middle() - item.get_level().get_camera_center().y) /
      item.get_level().get_camera()->get_height() );

  entity* e = dynamic_cast<entity*>(&item);
  
  if ( e != NULL ) 
    {
      unsigned int combo = e->get_combo_value();

      if (  combo > 0 )
        {
          game_variables::add_score( combo, points, true );
        
          if ( combo != game_variables::get_last_combo() )
            game_variables::set_last_combo( combo );
        }
    }
  else
    game_variables::add_score( 1, points, true );
} // util::create_floating_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create some smoke.
 * \param ref The item on which the smoke is created.
 * \param count The number of smoke clouds to create.
 * \param min_intensity The minimum intensity of the clouds.
 * \param max_intensity The maximum intensity of the clouds.
 * \param z_shift The z position of the smoke, relatively to the reference.
 */
void rp::util::create_smoke
( const bear::engine::base_item& ref, std::size_t count,
  double min_intensity, double max_intensity, int z_shift )
{
  for ( std::size_t i(0); i!=count; ++i )
    create_smoke( ref, min_intensity, max_intensity, z_shift );
} // util::create_smoke()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load game variables.
 */
void rp::util::load_game_variables()
{
  const bear::engine::game& g( bear::engine::game::get_instance() );

  const std::string filename
    ( g.get_game_filesystem().get_custom_config_file_name( RP_SAVE_FILENAME ) );
 
  std::ifstream f( filename.c_str() );
  bear::engine::var_map vars;
  bear::engine::variable_list_reader reader;

  reader(f, vars);

  bear::engine::game::get_instance().set_game_variables(vars);
} // util::load_game_variables()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save all game variables.
 */
void rp::util::save_game_variables()
{
  const bear::engine::game& g( bear::engine::game::get_instance() );

  const std::string filename
    ( g.get_game_filesystem().get_custom_config_file_name( RP_SAVE_FILENAME ) );

  std::ofstream f( filename.c_str() );

  bear::engine::game::get_instance().save_game_variables(f, "persistent/.*");
} // util::save_game_variables()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a star behing the cart.
 * \param ref The item around which the star is created.
 * \param border_color The color of the border of the star.
 * \param fill_color The color inside the star.
 * \param time_to_live How long the star stays alive.
 */
void rp::util::create_hit_star
( bear::engine::base_item& ref, bear::visual::color_type border_color,
  bear::visual::color_type fill_color,
  bear::universe::time_type time_to_live )
{
  bear::star* item = new bear::star( 30, 0.9, border_color, 3, fill_color );

  const bear::visual::size_type size
    ( 2 * std::max( ref.get_width(), ref.get_height() ) );
  item->set_size( size, size );

  item->set_z_position( ref.get_z_position() - 1 );
  item->set_system_angle_as_visual_angle( true );

  ref.new_item(*item);

  item->set_center_of_mass( ref.get_center_of_mass() );
  
  bear::universe::forced_rotation m;
  m.set_reference_point_on_center( ref );
  m.set_loops(0);
  m.set_acceleration_time( 0 );
  m.set_total_time( 4 );
  m.set_radius( 0 );
  m.set_angle_application( bear::universe::forced_rotation::apply_force );

  item->set_forced_movement( m );
  
  bear::delayed_kill_item* k = new bear::delayed_kill_item;
  k->add_item( item );
  k->set_duration( time_to_live );
  
  ref.new_item(*k);
} // util::create_hit_star()

/*----------------------------------------------------------------------------*/
/**
 * \brief Send statistic information when the player quit a level.
 * \param filename The filename of the level.
 */
void rp::util::send_quit_level( const std::string& filename )
{
  std::list<bear::engine::stat_variable> vars;
 
  vars.push_back
    ( bear::engine::stat_variable( "level", filename ) );

  bear::engine::game::get_instance().send_data( "quit-level", vars );
} // util::send_quit_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Send statistic information when a level is complete.
 * \param filename The filename of the level.
 */
void rp::util::send_complete_level( const std::string& filename )
{
  std::list<bear::engine::stat_variable> vars;

  vars.push_back
    ( bear::engine::stat_variable( "level", filename ) );
  vars.push_back
    ( bear::engine::stat_variable
      ( "medal", game_variables::get_last_medal() ) );
  vars.push_back
    ( bear::engine::stat_variable
      ( "balloon", game_variables::get_balloons_number() ) ); 
  vars.push_back
    ( bear::engine::stat_variable
      ( "score", game_variables::get_score() ) );

  bear::engine::game::get_instance().send_data( "complete-level", vars );
} // util::send_complete_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Send statistic information when the player fails a level.
 * \param filename The filename of the level.
 */
void rp::util::send_fail_level( const std::string& filename )
{
  std::list<bear::engine::stat_variable> vars;

  vars.push_back
    ( bear::engine::stat_variable( "level", filename ) );
  vars.push_back
    ( bear::engine::stat_variable
      ( "balloon", game_variables::get_balloons_number() ) );
  vars.push_back
    ( bear::engine::stat_variable
      ( "required-balloon", game_variables::get_required_balloons_number() ) );

  bear::engine::game::get_instance().send_data( "fail-level", vars );
} // util::send_fail_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Send statistic information when the player dies in a level.
 * \param filename The filename of the level.
 */
void rp::util::send_die_level( const std::string& filename )
{
  std::list<bear::engine::stat_variable> vars;

  vars.push_back
    ( bear::engine::stat_variable( "level", filename ) );

  bear::engine::game::get_instance().send_data( "die-level", vars );
} // util::send_die_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Send statistic information baout version.
 */
void rp::util::send_version()
{
  std::list<bear::engine::stat_variable> vars;

  vars.push_back( bear::engine::stat_variable( "major", RP_MAJOR_VERSION ) );
  vars.push_back( bear::engine::stat_variable( "minor", RP_MINOR_VERSION ) );
  vars.push_back( bear::engine::stat_variable( "patch", RP_PATCH_NUMBER ) ); 

  bear::engine::game::get_instance().send_data( "version", vars );
} // util::send_version()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create some smoke.
 * \param ref The item on which the smoke is created.
 * \param min_intensity The minimum intensity of the clouds.
 * \param max_intensity The maximum intensity of the clouds.
 * \param z_shift The z position of the smoke, relatively to the reference.
 */
void rp::util::create_smoke
( const bear::engine::base_item& ref, double min_intensity,
  double max_intensity, int z_shift )
{
  bear::visual::animation anim
    ( ref.get_level_globals().get_animation("animation/effect/steam.canim") );
  anim.set_time_factor( 1 + 3 * (double)rand() / RAND_MAX );

  const double intensity =
    min_intensity + (max_intensity - min_intensity) * rand() / RAND_MAX;
  anim.set_intensity( intensity, intensity, intensity );

  bear::decorative_item* item = new bear::decorative_item;

  item->set_mass( 1 );
  item->set_density( 0.0006 + 0.0002 * rand() / RAND_MAX );

  item->set_animation( anim );
  item->set_z_position( ref.get_z_position() + z_shift );
  item->set_kill_when_leaving( true );
  item->set_kill_when_finished( true );
  item->get_rendering_attributes().set_angle( 6.29 * rand() / RAND_MAX );

  const bear::universe::position_type pos
    ( ref.get_width() * rand() / RAND_MAX,
      ref.get_height() * rand() / RAND_MAX );

  item->set_center_of_mass( ref.get_bottom_left() + pos );
  
  ref.new_item( *item );
  
  apply_random_smoke_effect( *item );

  CLAW_ASSERT
    ( item->is_valid(), 
      "The decoration of explosion isn't correctly initialized" );
} // util::create_smoke()

/*----------------------------------------------------------------------------*/
/**
 * \brief Applies a random effect to a smoke cloud.
 * \param item The smoke cloud.
 */
void rp::util::apply_random_smoke_effect( bear::engine::base_item& item )
{
  bear::decorative_effect* effect = new bear::decorative_effect();

  effect->set_size_factor_end( 0.2 + 0.5 * rand() / RAND_MAX );
  bear::visual::color init_color, end_color;
  init_color.set(1,1,1,0.8 + 0.2 * rand() / RAND_MAX);
  end_color.set(1,1,1,0.8 * rand() / RAND_MAX);
  effect->set_color( init_color, end_color );

#if defined( __ANDROID__ )
  effect->set_duration( 1 );
#else
  effect->set_duration( 2 );
#endif

  effect->set_item( &item, true );

  item.new_item( *effect );
} // util::apply_random_smoke_effect()

/*----------------------------------------------------------------------------*/
/**
 * \brief Saves the current configuration.
 */
void rp::util::save_config()
{
  rp::config_file config;
  config.update();
  config.save();
} // util::save_config()
