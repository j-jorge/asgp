/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \brief Implementation of the rp::config_file class.
 * \author Julien Jorge
 */
#include "rp/config_file.hpp"

#include <claw/configuration_file.hpp>
#include <sstream>
#include <fstream>

#include "engine/game.hpp"
#include "engine/system/game_filesystem.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::config_file::config_file()
  : m_fullscreen(false),
    m_dumb_rendering(true),
    m_sound_on(true),
    m_sound_volume(1),
#ifdef __ANDROID__
    m_music_on(false),
#else
    m_music_on(true),
#endif
    m_music_volume(0.7),
    m_config_name("config")
{
  const bear::engine::game& g( bear::engine::game::get_instance() );

  std::string full_config_path
    ( g.get_game_filesystem().get_custom_config_file_name(m_config_name) );
  std::ifstream f( full_config_path.c_str() );

  if (f)
    {
      claw::configuration_file config(f);

      str_to_bool( m_fullscreen, config("Video", "fullscreen") );
      str_to_bool( m_dumb_rendering, config("Video", "dumb_rendering") );
      str_to_bool( m_sound_on, config("Audio", "sound_on") );
      str_to_bool( m_music_on, config("Audio", "music_on") );

      str_to_double( m_music_volume, config("Audio", "music_volume") );
      str_to_double( m_sound_volume, config("Audio", "sound_volume") );
    }
} // config_file::config_file

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply the configuration to the game.
 */
void rp::config_file::apply() const
{
  bear::engine::game::get_instance().set_fullscreen( m_fullscreen );
  bear::engine::game::get_instance().set_dumb_rendering( m_dumb_rendering );
  bear::engine::game::get_instance().set_sound_muted( !m_sound_on );
  bear::engine::game::get_instance().set_music_muted( !m_music_on );
  bear::engine::game::get_instance().set_sound_volume( m_sound_volume );
  bear::engine::game::get_instance().set_music_volume( m_music_volume );
} // config_file::apply()

/*----------------------------------------------------------------------------*/
/**
 * \brief Updates the configuration according to the current game options.
 */
void rp::config_file::update()
{
  m_fullscreen = bear::engine::game::get_instance().get_fullscreen();
  m_dumb_rendering = bear::engine::game::get_instance().get_dumb_rendering();
  m_sound_on = !bear::engine::game::get_instance().get_sound_muted();
  m_music_on = !bear::engine::game::get_instance().get_music_muted();
  m_sound_volume = bear::engine::game::get_instance().get_sound_volume();
  m_music_volume = bear::engine::game::get_instance().get_music_volume();
} // config_file::update()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the configuration.
 */
void rp::config_file::save() const
{
  const bear::engine::game& g( bear::engine::game::get_instance() );

  std::string full_config_path
    ( g.get_game_filesystem().get_custom_config_file_name(m_config_name) );
  std::ofstream f( full_config_path.c_str() );

  f << "# Configuration of the screen.\n"
    << "[Video]\n"
    << "# Do we use the fullscreen?\n"
    << "fullscreen = " << bool_to_str(m_fullscreen) << '\n'
    << "# Do we use the dumb but visually better procedure to render the "
    "elements?\n"
    << "dumb_rendering = " << bool_to_str(m_dumb_rendering) << '\n'
    << '\n'
    << "# Configuration of the sound system.\n"
    << "[Audio]\n"
    << "# Do we play the sounds?\n"
    << "sound_on = " << bool_to_str(m_sound_on) << '\n'
    << "# Do we play the music?\n"
    << "music_on = " << bool_to_str(m_music_on) << '\n'
    << "# The volume of the sounds\n"
    << "sound_volume = " << m_sound_volume << '\n'
    << "# The volume of the music music\n"
    << "music_volume = " << m_music_volume << '\n'
    << std::endl;
} // config_file::save()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the fullscreen mode value.
 * \param b The new value.
 */
void rp::config_file::set_fullscreen( bool b )
{
  m_fullscreen = b;
} // config_file::set_fullscreen()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the sound mode value.
 * \param b The new value.
 */
void rp::config_file::set_sound_on( bool b )
{
  m_sound_on = b;
} // config_file::set_sound_on()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the music mode value.
 * \param b The new value.
 */
void rp::config_file::set_music_on( bool b )
{
  m_music_on = b;
} // config_file::set_music_on()

/*----------------------------------------------------------------------------*/
/**
 * \brief Convert a boolean value to its string respresentation.
 * \param b The boolean to convert.
 */
std::string rp::config_file::bool_to_str( bool b ) const
{
  if (b)
    return "true";
  else
    return "false";
} // config_file::bool_to_str()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get a boolean value from its string representation.
 * \param b The resulting boolean.
 * \param s The string representation.
 * \remark \a b is unchanged if \æ s does not represent a boolean value.
 */
void rp::config_file::str_to_bool( bool& b, const std::string& s ) const
{
  b = (s != "false") && (s != "0");
} // config_file::str_to_bool()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get a double value from its string representation.
 * \param d The resulting double.
 * \param s The string representation.
 * \remark \a d is unchanged if \æ s does not represent a double value.
 */
void rp::config_file::str_to_double( double& d, const std::string& s ) const
{
  double result;
  std::istringstream iss(s);

  if ( iss >> result )
    d = result;
} // config_file::str_to_double()
