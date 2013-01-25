/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The component to display the lives bonus state.
 * \author Sebastien Angibaud
 */
#ifndef __RP_LIVES_COMPONENT_HPP__
#define __RP_LIVES_COMPONENT_HPP__

#include "rp/layer/status/status_component.hpp"

#include "visual/animation.hpp"

namespace rp
{
  /**
   * \brief The component to display the lives bonus state.
   * \author Sebastien Angibaud
   */
  class lives_component :
    public status_component
  {
  public:
    /** \brief The type of the parent class. */
    typedef status_component super;

  public:
    lives_component
    ( bear::engine::level_globals& glob, 
      const bear::visual::position_type& active_position,
      bear::universe::zone::position side,
      x_placement x_p, y_placement y_p,
      const bear::universe::size_box_type& layer_size,
      const bear::universe::coordinate_type& hide_height, bool flip);

    void build();
    void render( scene_element_list& e ) const;
    unsigned int width() const;
    unsigned int height() const;

  protected:
    void init_signals();

  private:
    void on_lives_changed(unsigned int value);      

  private:
    /** \brief The current sprite of lives bonus. */
    bear::visual::sprite m_sprite;

  }; // class lives_component
} // namespace rp

#endif // __RP_LIVES_COMPONENT_HPP__
