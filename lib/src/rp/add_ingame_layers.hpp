/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief This class adds the layers for the graphical user interface in the
 *        current level.
 * \author Julien Jorge
 */
#ifndef __RP_ADD_INGAME_LAYERS_HPP__
#define __RP_ADD_INGAME_LAYERS_HPP__

#include "engine/base_item.hpp"
#include "engine/layer/gui_layer.hpp"
#include "engine/export.hpp"
#include "generic_items/timer.hpp"

namespace rp
{
  /**
   * \brief This class adds the layers for the graphical user interface in the
   *        current level.
   *
   * The custom fields of this class are
   * - show_introduction (boolean) tell if we must show the introduction of
   *   the level (with the level name and the act number) (default=true),
   *
   * \author Julien Jorge
   */
  class add_ingame_layers:
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM(add_ingame_layers);

  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::base_item super;

  public:
    add_ingame_layers();

    void pre_cache();
    void build();

    bool set_bool_field( const std::string& name, bool value );
    void set_level_timer(const bear::timer* level_timer);

  private:
    /** \brief Tells if we must show the introduction of the level. */
    bool m_add_starting_effect;

    /** \brief Tells if we must add status layer. */
    bool m_add_status_layer;

    /** \brief Tells if we must add key layer. */
    bool m_add_key_layer;

    /** \brief A pointer on status layer. */
    const bear::timer* m_level_timer;

  }; // class add_ingame_layers
} // namespace rp

#endif // __RP_ADD_INGAME_LAYERS_HPP__
