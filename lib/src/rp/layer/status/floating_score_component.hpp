/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief A class that contains floating_score' information.
 * \author Sebastien Angibaud
 */
#ifndef __RP_FLOATING_SCORE_HPP__
#define __RP_FLOATING_SCORE_HPP__

#include "rp/layer/status/status_component.hpp"
#include "visual/writing.hpp"

#include "universe/types.hpp"
#include "visual/animation.hpp"
#include <claw/tween/tweener_group.hpp>

namespace rp
{
  /**
   * \brief A class that contains floating_score' informations.
   * \author Sebastien Angibaud
   */
  class floating_score_component
  {
  public:
    /** \brief The type of the parent class. */
    typedef status_component super;
 
  public:
    floating_score_component(bear::engine::level_globals& glob);
    void progress(bear::universe::time_type elapsed_time);
    void render( status_component::scene_element_list& e ) const;
    bool is_finished() const;

    const bear::visual::position_type&  get_position() const;
    void set_position
    ( const bear::visual::position_type& init_position,
      const bear::visual::position_type& end_position );
    unsigned int get_combo_value() const;
    int get_points_value() const;
    
  private:
    void on_x_position_update(double x);
    void on_y_position_update(double x);

  private:
    /** \brief The set of current tweeners. */
    claw::tween::tweener_group m_tweeners;
    
    /** \brief Position of the floating corrupting bonus. */
    bear::visual::position_type m_position;

    /** \brief The font for text. */
    bear::visual::font m_font;
    
    /** \brief The points text. */
    bear::visual::writing m_points;

    /** \brief The combo text. */
    bear::visual::writing m_combo;

    /** \brief The sprite. */
    bear::visual::sprite m_sprite;

    /** \brief The value of the combo. */
    unsigned int m_combo_value;

    /** \brief The value of points. */
    int m_points_value;
  }; // class floating_score
} // namespace rp

#endif // __RP_FLOATING_SCORE_HPP__
