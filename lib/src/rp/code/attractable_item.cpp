/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the attractable_item class.
 * \author Sebastien Angibaud
 */
#include "rp/attractable_item.hpp"
#include "rp/cart.hpp"
#include "rp/plunger.hpp" 
#include "rp/util.hpp"

#include "universe/forced_movement/forced_tracking.hpp"

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::attractable_item::attractable_item()
  : m_attracted(false), m_taken(false), m_plunger(NULL)
{

} // rp::attractable_item::attractable_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform that the item is attracted by the plunger.
 * \param p The plunger that attracts.
 */
void rp::attractable_item::attract(plunger* p)
{
  m_attracted = true;

  if ( p != NULL )
    {
      m_plunger = p;
      bear::universe::forced_tracking mvt;
      
      mvt.set_reference_point_on_center(*p);
      mvt.set_distance(get_center_of_mass() - p->get_center_of_mass());
      
      set_forced_movement( mvt );
      set_combo_value(1);
    }
} // attractable_item::attract()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when cart takes the object.
 * \param c The cart that take the object
 */
void rp::attractable_item::be_taken(cart * c)
{
  m_taken = true;
  m_attracted = false;
  leave();
} // attractable_item::be_taken()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the attracted state.
 */
bool rp::attractable_item::get_attracted_state() const
{
  return m_attracted;
} // attractable_item::get_attracted_state()
   
/*----------------------------------------------------------------------------*/
/**
 * \brief Get the taken state.
 */
bool rp::attractable_item::get_taken_state() const
{
  return m_taken;
} // attractable_item::get_taken_state()
   
/*----------------------------------------------------------------------------*/
/**
 * \brief Indicates if the item can be attractable.
 */
bool rp::attractable_item::is_attractable() const
{
  return ! get_attracted_state();
} // attractable_item::is_attractable()

/*----------------------------------------------------------------------------*/
/**
 * \brief The object leaves the plunger.
 */
void rp::attractable_item::leave()
{
  if ( m_plunger != NULL )
    m_plunger->leave();

  m_plunger = NULL;
} // attractable_item::leave()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::attractable_item::populate_loader_map
( bear::engine::item_loader_map& m )
{
  super::populate_loader_map( m );

  m.insert( entity::loader( *this ) );
} // attractable_item::populate_loader_map()
