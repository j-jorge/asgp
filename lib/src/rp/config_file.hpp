/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \brief The class storing the information of the configuration file of the
 *        game.
 * \author Julien Jorge
 */
#ifndef __RP_CONFIG_FILE_HPP__
#define __RP_CONFIG_FILE_HPP__

#include <string>

namespace rp
{
  /**
   * \brief The class storing the information of the configuration file of the
   *        game.
   * \author Julien Jorge
   */
  class config_file
  {
  public:
    config_file();

    void apply() const;
    void update();
    void save() const;

    void set_fullscreen( bool b );
    void set_sound_on( bool b );
    void set_music_on( bool b );

  private:
    std::string bool_to_str( bool b ) const;
    void str_to_bool( bool& b, const std::string& s ) const;
    void str_to_double( double& d, const std::string& s ) const;

  private:
    /** \brief Tells if we use the fullscreen mode. */
    bool m_fullscreen;

    /** \brief Tells if we use the dumb rendering mode. */
    bool m_dumb_rendering;

    /** \brief Tells if we turn on the sounds. */
    bool m_sound_on;

    /** \brief The volume of the sounds. */
    double m_sound_volume;

    /** \brief Tells if we turn on the music. */
    bool m_music_on;

    /** \brief The volume of the musics. */
    double m_music_volume;

    /** \brief The name of the configuration file. */
    const std::string m_config_name;

  }; // class config_file
} // namespace rp

#endif // __RP_CONFIG_FILE_HPP__
