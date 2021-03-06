/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The layer displayed when the user wants to buy the game.
 * \author Julien Jorge
 */
#ifndef __RP_KEY_LAYER_HPP__
#define __RP_KEY_LAYER_HPP__

#include "communication/messageable.hpp"
#include "engine/layer/gui_layer.hpp"

#include "gui/button.hpp"
#include "gui/text_input.hpp"

namespace rp
{
  /**
   * \brief The layer displayed when the user wants to buy the game.
   * \author Julien Jorge
   */
  class key_layer:
    public bear::communication::messageable,
    public bear::engine::gui_layer
  {
  private:
    /** \brief The type of the main super class. */
    typedef bear::engine::gui_layer super;

  public:
    key_layer( const std::string& name );

    void build();

    bool key_pressed( const bear::input::key_info& key );

    void show();
    void hide();

  private:
    void create_components();

    void create_key_text();
    void create_label();
    void create_validate_button();
    void create_cancel_button();

    void update_validate_button();
    void validate();

  private:
    /** \brief The window containing the components of the layer. */
    bear::gui::visual_component m_root_window;

    /** \brief Indicates if the layer is visible. */
    bool m_visible;

    /** \brief The component in which the user can enter his key. */
    bear::gui::text_input* m_key_text;

    /** \brief The button to click to validate the key.. */
    bear::gui::button* m_ok_button;

  }; // class key_layer
} // namespace rp

#endif // __RP_KEY_LAYER_HPP__
