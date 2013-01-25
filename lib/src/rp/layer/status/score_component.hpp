/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The component to display the score.
 * \author Sebastien Angibaud
 */
#ifndef __RP_SCORE_COMPONENT_HPP__
#define __RP_SCORE_COMPONENT_HPP__

#include "rp/layer/status/status_component.hpp"

#include "visual/writing.hpp"

namespace rp
{
  class floating_score_component;

  /**
   * \brief The component to display the score.
   * \author Sebastien Angibaud
   */
  class score_component :
    public status_component
  {
  public:
    /** \brief The type of the parent class. */
    typedef status_component super;
 
  private:
    /** \brief The type of a list of floating score. */
    typedef std::list<floating_score_component> floating_score_list;
 
  public:
    score_component
    ( bear::engine::level_globals& glob, 
      const bear::visual::position_type& active_position,
      bear::universe::zone::position side,
      x_placement x_p, y_placement y_p,
      const bear::universe::size_box_type& layer_size,
      const bear::universe::coordinate_type& hide_height, bool flip);

    void build();
    void progress( bear::universe::time_type elapsed_time );
    void render( scene_element_list& e ) const;
    unsigned int width() const;
    unsigned int height() const;

  protected:
    void init_signals();

  private:
    void on_score_added(unsigned int combo, int points);  
    void create_tweener();
    void on_new_score(bool value);

  private:
    /** \brief The font for text. */
    bear::visual::font m_font;
    
    /** \brief The score. */
    bear::visual::writing m_score;

    /** \brief The background sprite. */
    bear::visual::sprite m_background;

    /** \brief map of floating score. */
    floating_score_list m_floating_score;
  }; // class score_component
} // namespace rp

#endif // __RP_SCORE_COMPONENT_HPP__
