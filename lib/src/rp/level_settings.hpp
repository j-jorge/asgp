/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief An item to set parameters of level.
 * \author Angibaud Sebastien
 */
#ifndef __RP_LEVEL_SETTINGS_HPP__
#define __RP_LEVEL_SETTINGS_HPP__

#include "generic_items/level_settings.hpp"
#include "engine/export.hpp"

namespace rp
{
  class cart;
  class add_ingame_layers;

  /**
   * \brief An item to set parameters of level.
   *
   * The valid fields for this item are
   *  \c (unsigned int) level_duration The duration of the level.
   *  \c (bool) is_boss_level Indicates if the level is a boss level.
   *  \c (string) level_info An optionnal indication for level.
   *  \c (string) level_theme The theme of the level.
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class level_settings:
    public bear::level_settings
  {
    DECLARE_BASE_ITEM(level_settings);

  public:
    /** \brief The type of the parent class. */
    typedef bear::level_settings super;

  public:
    level_settings();

    void build();
    void on_enters_layer();
    
    bool set_real_field( const std::string& name, double value );
    bool set_bool_field( const std::string& name, bool value );
    bool set_item_field
    ( const std::string& name, bear::engine::base_item* value );
    bool set_string_field
    ( const std::string& name, const std::string& value );

  private:
    /** \brief The cart of the level. */
    cart* m_cart;    

    /** \brief The add_ingame_layers of the level. */
    add_ingame_layers* m_add_ingame_layers;

    /** \brief The duration of the level. */
    double m_level_duration;

    /** \brief An optionnal indication for level. */
    std::string m_level_info;

    /** \brief The theme of the level. */
    std::string m_level_theme;

    /** \brief Indicates if the level is a boss level. */
    bool m_boss_level;    

    /** \brief Indicates if the level is a boss transition. */
    bool m_boss_transition;
  }; // class level_settings
} // namespace rp

#endif // __RP_LEVEL_SETTINGS_HPP__
