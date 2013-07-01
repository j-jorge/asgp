/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The layer displayed when the game is paused.
 * \author Julien Jorge
 */
#ifndef __RP_PAUSE_LAYER_HPP__
#define __RP_PAUSE_LAYER_HPP__

#include "communication/messageable.hpp"
#include "engine/layer/gui_layer.hpp"

#include "gui/visual_component.hpp"
#include "gui/picture.hpp"

#include <list>
#include <map>

namespace rp
{
  /**
   * \brief The layer displayed when the game is paused.
   * \author Julien Jorge
   */
  class pause_layer:
    public bear::communication::messageable,
    public bear::engine::gui_layer
  {
  private:
    /** \brief The type of the main super class. */
    typedef bear::engine::gui_layer super;

    /** \brief The type of a list of controls in the layer. */
    typedef std::vector<bear::gui::visual_component*> control_list;

  public:
    pause_layer( const std::string& name );

    void build();

    void set_pause( bool pause_on );

    bool key_pressed( const bear::input::key_info& key );
    bool mouse_move( const claw::math::coordinate_2d<unsigned int>& pos );

  private:
    void add_level_name_component();
    void add_title_component();
    void add_quit_resume_components();
    void add_system_buttons();

    bear::gui::visual_component* create_sound_component();
    bear::gui::visual_component* create_help_component();
    bear::gui::visual_component* create_music_component();
    bear::gui::visual_component* create_fullscreen_component();

    void create_focus_background_component( bear::gui::visual_component* c );

    bear::visual::sprite get_focus_on_background() const;
    bear::visual::sprite get_focus_off_background() const;

    void add_component( bear::gui::visual_component* c );

    void highlight_component( bear::gui::visual_component* c );

    void on_quit();
    void on_help();

  private:
    /** \brief The window containing the components of the layer. */
    bear::gui::visual_component m_root_window;

    /** \brief The margin between the borders and the elements. */
    bear::gui::coordinate_type m_margin;

    /** \brief The interactive controls in the layer. */
    control_list m_controls;

    /** \brief The currently focused component. */
    bear::gui::visual_component* m_active_component;

    /** \brief The component placed behind the focused component. */
    std::map< bear::gui::visual_component*, bear::gui::picture* >
    m_focus_background;

  }; // class pause_layer
} // namespace rp

#endif // __RP_PAUSE_LAYER_HPP__
