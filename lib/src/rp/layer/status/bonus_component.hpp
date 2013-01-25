/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief A component that displays the state of the plunger bonus.
 * \author Julien Jorge
 */
#ifndef __RP_BONUS_COMPONENT_HPP__
#define __RP_BONUS_COMPONENT_HPP__

#include "rp/layer/status/status_component.hpp"

#include "visual/sprite.hpp"

#include <claw/tween/tweener.hpp>

namespace rp
{
  /**
   * \brief A component that displays the state of the plunger bonus.
   * \author Julien Jorge
   */
  class bonus_component :
    public status_component
  {
  public:
    /** \brief The type of the parent class. */
    typedef status_component super;

  public:
    bonus_component
    ( bear::engine::level_globals& glob, 
      const bear::visual::position_type& active_position,
      bear::universe::zone::position side,
      x_placement x_p, y_placement y_p,
      const bear::universe::size_box_type& layer_size,
      const bear::universe::coordinate_type& hide_height, bool flip);

    void build();
    void render( scene_element_list& e ) const;
    void progress( bear::universe::time_type elapsed_time );

    unsigned int width() const;
    unsigned int height() const;

  protected:
    void init_signals();

  private:
    void on_bonus_activation_changed( unsigned int value );
    void on_activation_update( double value );

  private:
    /** \brief The sprite representing the state of the plunger bonus. */
    bear::visual::sprite m_sprite;

    /** \brief The tweener that produce the effect applied when the bonus is
        activated. */
    claw::tween::tweener m_effect_tweener;

  }; // class bonus_component
} // namespace rp

#endif // __RP_BONUS_COMPONENT_HPP__
