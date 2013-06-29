/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The effect displayed at the introduction of the levels.
 * \author Sébastien Angibaud
 */
#ifndef __RP_LEVEL_STARTING_EFFECT_HPP__
#define __RP_LEVEL_STARTING_EFFECT_HPP__

#include "visual/writing.hpp"
#include "engine/transition_effect/transition_effect.hpp"

#include <claw/tween/tweener_sequence.hpp>
#include <claw/tween/single_tweener.hpp>

namespace rp
{
  /**
   * \brief The effect displayed at the introduction of the levels.
   * \author Sébastien Angibaud
   */
  class level_starting_effect:
    public bear::engine::transition_effect
  {
  public:
    /** \brief The type of a list of scene elements retrieved from the layer. */
    typedef std::list<bear::visual::scene_element> scene_element_list;

  public:
    level_starting_effect();

    bool is_finished() const;
    void build();
    bear::universe::time_type
    progress( bear::universe::time_type elapsed_time );
    void render( scene_element_list& e ) const;

  private:
    bear::visual::coordinate_type render_panel
    ( scene_element_list& e, const bear::visual::position_type& center ) const;
    void render_opaque_rectangle( scene_element_list& e ) const;
    void render_level_name
    ( scene_element_list& e, const bear::visual::position_type& center,
      const bear::visual::coordinate_type& y_panel ) const;
    void render_balloon_text
    ( scene_element_list& e, const bear::visual::position_type& center,
      const bear::visual::coordinate_type& y_panel ) const;
    void render_level_information
    ( scene_element_list& e, const bear::visual::position_type& center ) const;
    void create_controls();
    void update_balloons_number();
    void create_balloons_tweener();
    void create_fade_in_tweener();
    void end_effect();

  private:
    /** \brief How long the layer stays visible. */
    const bear::universe::time_type m_visibility_duration;

    /** \brief The time before counter starting. */
    const bear::universe::time_type m_start_time;

    /** \brief Elapsed time since the beginning */
    bear::universe::time_type m_elapsed_time;

    /** \brief The duration of the counter. */
    const bear::universe::time_type m_counter_duration;

    /** \brief The name of the level. */
    bear::visual::writing m_level_name;

    /** \brief The number of the required balloons. */
    bear::visual::writing m_balloons;
    
    /** \brief The number of the required balloons. */
    unsigned int m_nb_balloons;

    /** \brief The scale for required balloons. */
    double m_balloons_scale;

     /** \brief The factor intensity of balloon text. */
    double m_factor_intensity;

    /** \brief The background picture. */
    bear::visual::sprite m_background;

    /** \brief The centered text. */
    bear::visual::writing m_text;

    /** \brief The text containing level info. */
    bear::visual::writing m_info;

    /** \brief The tweener for balloons writing scale. */
    claw::tween::tweener_sequence m_tweener_scale;

    /** \brief The opacity of the fade in. */
    double m_fade_in_opacity;    

    /** \brief The tweener for fade_in opacity. */
    claw::tween::single_tweener m_tweener_fade_in;   

    /** \brief The tweener for gap_y of level_name. */
    claw::tween::tweener_sequence m_tweener_level_name_gap_y;

    /** \brief The tweener for gap_y of panel. */
    claw::tween::tweener_sequence m_tweener_panel_gap_y;

    /** \brief Duration of the fade out . */
    bear::universe::time_type m_fade_out_duration;

    /** \brief The gap_y of level_name. */
    double m_level_name_gap_y;

     /** \brief The gap_y of panel. */
    double m_panel_gap_y;

  }; // class level_starting_effect
} // namespace rp

#endif // __RP_LEVEL_STARTING_EFFECT_HPP__
