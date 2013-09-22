/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing an entity.
 * \author Sebastien Angibaud
 */
#ifndef __RP_ENTITY_HPP__
#define __RP_ENTITY_HPP__

#include "engine/base_item.hpp"

#include <vector>
#include <string>

namespace rp
{
  class interactive_item;
  
  /**
   * \brief The class describing an entity.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class entity
  {
  public:
    /**
     * \brief The item loader for an entity.
     */
    class loader:
      public bear::engine::item_loader_base
    {
    private:
      /** \brief The type of the parent class. */
      typedef bear::engine::item_loader_base super;
      
    public:
      explicit loader( entity& item );

      loader* clone() const;

      bool set_field( const std::string& name, bool value );
      bool set_field
      ( const std::string& name, const bear::visual::animation& value );

    private:
      /** \brief The instance loaded by this loader. */
      entity& m_item;

    }; // class loader

  public:
    entity();
    
    // transportabily by the boss
    void set_transportability(bool value);
    bool is_transportable() const;

    // attack point
    virtual bool has_attack_point() const;
    virtual bear::universe::position_type get_attack_point() const;
    
    // interactive item
    void create_interactive_item
    ( bear::engine::base_item& item, double factor_sprite = 1, 
      double area_factor = 0.5,
      bear::universe::position_type gap_x = 
      bear::universe::position_type( 0, 0 ) );
    interactive_item* get_interactive_item();
    void kill_interactive_item();

    // combo
    void set_combo_value(unsigned int value);
    void update_combo_value(entity* item);
    unsigned int get_combo_value() const;
    std::string give_note();

    // passive state
    bool get_passive() const;
    void set_passive( bool value );

    void set_help_animation( const bear::visual::animation& anim );

  protected:
    void populate_loader_map( bear::engine::item_loader_map& m );

  private:
    /* \brief Value of the combo. */
    unsigned int m_combo_value;

    /** \brief A pointer on the interactive item. */
    interactive_item* m_interactive_item;

    /** Indicates that the entity can be transported by the boss. */
    bool m_transportable;

    /** Indicates that the entity is passive. */
    bool m_passive;

    /** \brief The animation showing the player how to shoot the item. */
    bear::visual::animation m_help;

    /* \brief The index in melody. */
    static unsigned int s_combo_note;
  }; // class entity
} // namespace rp

#endif // __RP_ENTITY_HPP__
