/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a boss_teleport.
 * \author Sebastien Angibaud
 */
#ifndef __RP_BOSS_TELEPORT_HPP__
#define __RP_BOSS_TELEPORT_HPP__

#include "rp/boss.hpp"
#include "generic_items/teleport_item.hpp"
#include "engine/export.hpp"

namespace rp
{
  class boss;

  /**
   * \brief The class describing a boss_teleport.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class boss_teleport :
    public bear::teleport_item
  {
    DECLARE_BASE_ITEM(boss_teleport);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::teleport_item super;

  public:
    boss_teleport();

    bool set_item_field
    ( const std::string& name, bear::engine::base_item* value);
    void get_dependent_items
    ( bear::universe::physical_item::item_list& d ) const;
    
  protected:
    void on_toggle_on( bear::engine::base_item* activator );

  private:
    /** \brief The reference item for y-coordinate. */
    boss::handle_type m_y_reference_item;
    
    /** \brief The boss of the level. */
    boss* m_boss;
  }; // class boss_teleport
} // namespace rp

#endif // __RP_BOSS_TELEPORT_HPP__
