/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The layer displayed when the game display the help.
 * \author Sébastien Angibaud
 */
#ifndef __RP_HELP_LAYER_HPP__
#define __RP_HELP_LAYER_HPP__

#include "communication/messageable.hpp"
#include "engine/layer/gui_layer.hpp"
#include "gui/visual_component.hpp"

namespace rp
{
  /**
   * \brief The layer displayed when the game display the help.
   * \author Sébastien Angibaud
   */
  class help_layer:
    public bear::communication::messageable,
    public bear::engine::gui_layer
  {
  private:
    /** \brief The type of the main super class. */
    typedef bear::engine::gui_layer super;

  public:
    help_layer( const std::string& name );

    void build();

    bool key_pressed( const bear::input::key_info& key );
    bool mouse_pressed
    ( bear::input::mouse::mouse_code button,
      const claw::math::coordinate_2d<unsigned int>& pos );
    bool mouse_move
      ( const claw::math::coordinate_2d<unsigned int>& pos );
    void start();

  private:
    void set_help( bool help_on );
    void create_component();

  private:
    /** \brief The window containing the components of the layer. */
    bear::gui::visual_component m_root_window;

    /** \brief Indicates if the layer is active. */
    bool m_active;
  }; // class help_layer
} // namespace rp

#endif // __RP_HELP_LAYER_HPP__
