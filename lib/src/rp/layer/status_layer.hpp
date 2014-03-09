/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The status layer contains the area where are displayed the
 *        informations about the player.
 * \author Julien Jorge
 */
#ifndef __RP_STATUS_LAYER_HPP__
#define __RP_STATUS_LAYER_HPP__

#include "rp/layer/status/time_component.hpp"

#include "engine/layer/gui_layer.hpp"
#include "visual/sprite.hpp"
#include "visual/writing.hpp"

#include <boost/signals2.hpp>
#include <claw/tween/tweener_group.hpp>

namespace rp
{
  class cart;
  class status_component;

  /**
   * \brief The status layer contains the area where are displayed the
   *        informations about the player.
   */
  class status_layer:
    public bear::engine::gui_layer
  {
  public:
    /** \brief The type of a list of scene elements retrieved from the layer. */
    typedef bear::engine::gui_layer::scene_element_list scene_element_list;

    typedef bear::engine::gui_layer super;

    /** The A list of status component. */
    typedef std::list<status_component*> component_list;

  public:
    virtual ~status_layer();

    void build();
    void progress( bear::universe::time_type elapsed_time );
    void render( scene_element_list& e ) const;

  private:
    void create_components();

    void create_components_top_left();
    void create_components_bottom_right();

    template<typename T>
    T* create_status_component
    ( const bear::visual::position_type& active_position,
      bear::universe::zone::position side );

  private:
    /** \brief The component that display components. */
    component_list m_components;

    /** \brief The sprite of score background. */
    bear::visual::sprite m_score_background;

    /** \brief The margin around the sprites. */
    static const unsigned int s_margin;

  }; // class status_layer
} // namespace rp

#endif // __RP_STATUS_LAYER_HPP__
