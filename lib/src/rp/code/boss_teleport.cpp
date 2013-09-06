/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the boss_teleport class.
 * \author Sebastien Angibaud
 */
#include "rp/boss_teleport.hpp"
#include "rp/zeppelin.hpp"

#include "engine/item_brick/loader/activable_sides_loader.hpp"
#include "universe/collision_info.hpp"
#include "universe/zone.hpp"

BASE_ITEM_EXPORT( boss_teleport, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::boss_teleport::boss_teleport()
: m_y_reference_item(NULL), m_boss(NULL)
{
  set_size(50,1000);
} // rp::boss_teleport::rp::boss_teleport()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the value of a field of type item.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::boss_teleport::set_item_field
( const std::string& name, bear::engine::base_item* value)
{
  bool result(true);

  if ( name == "boss_teleport.y_reference_item" )
    m_y_reference_item = value;
  else if ( name == "boss_teleport.boss" )
    {
      m_boss = dynamic_cast<boss*>(value);
      
      if ( m_boss == NULL )
        claw::logger << claw::log_error << "boss_teleport::set_item_field:"
                     << " item is not an instance of 'rp::boss'."
                     << std::endl;
    }
  else
    result = super::set_item_field(name, value);

  return result;
} // boss_teleport::set_item_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the items concerned by a progress/move of this one.
 * \param d (out) A list to which are added such items.
 */
void rp::boss_teleport::get_dependent_items
( bear::universe::physical_item::item_list& d ) const
{
  super::get_dependent_items(d);

  if ( m_boss != NULL )
    d.push_back( m_boss );
} // boss_teleport::get_dependent_items()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the toggle is activated.
 * \param activator (ignored) The item that activates the toggle, if any.
 */
void rp::boss_teleport::on_toggle_on( bear::engine::base_item* activator )
{
  super::on_toggle_on(activator);

  if ( m_y_reference_item != boss::handle_type(NULL) )
    if ( m_boss != NULL )
      {
        m_boss->set_teleportation_gap(get_gap());
        m_boss->set_y_reference( m_y_reference_item->get_top() );
      }
} // boss::on_toggle_on()
