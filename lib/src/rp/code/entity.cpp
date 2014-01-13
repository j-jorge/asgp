/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the entity class.
 * \author Sebastien Angibaud
 */
#include "rp/entity.hpp"

#include "rp/game_variables.hpp"
#include "rp/interactive_item.hpp"

#include "universe/forced_movement/forced_tracking.hpp"
#include "universe/forced_movement/ratio_reference_point.hpp"

unsigned int rp::entity::s_combo_note = 0;


/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param item The instance loaded by this loaded.
 */
rp::entity::loader::loader( entity& item )
  : super("entity"), m_item(item)
{

} // entity::loader::loader()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates a copy of this instance.
 */
rp::entity::loader* rp::entity::loader::clone() const
{
  return new loader( *this );
} // entity::loader::clone()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets a field of type \c boolean.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::entity::loader::set_field( const std::string& name, bool value )
{
  bool result = true;

  if (name == "passive")
    m_item.set_passive( value );
  else
    result = false;

  return result;
} // entity::loader::set_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets a field of type \c animtation.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::entity::loader::set_field
( const std::string& name, const bear::visual::animation& value )
{
  bool result = true;

  if (name == "help")
    m_item.set_help_animation( value );
  else
    result = false;

  return result;
} // entity::loader::set_field()




/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::entity::entity()
  : m_combo_value(0), m_interactive_item(NULL), m_transportable(true),
    m_passive(false)
{
} // rp::entity::entity()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if the entity is transportable by the boss.
 */
void rp::entity::set_transportability( bool value )
{
  m_transportable = value;
} // rp::entity::set_transportability()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get if the entity is transportable by the boss.
 */
bool rp::entity::is_transportable() const
{
  return m_transportable;
} // rp::entity::is_transportable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the entity has a attack point.
 */
bool rp::entity::has_attack_point() const
{
  return false;
} // rp::entity::has_attack_point()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the attack point.
 */
bear::universe::position_type rp::entity::get_attack_point() const
{
  return bear::universe::position_type(0,0);
} // rp::entity::get_attack_point()

/*---------------------------------------------------------------------------*/
/**
 * \brief create a interactive item on an object.
 * \param item The item that creates the interactive item.
 * \param sprite_factor The factor of sprites.
 * \param area_factor The factor of required area for activation.
 * \param gap The gap according to the center of mass. 
 */
void rp::entity::create_interactive_item
( bear::engine::base_item& item, double factor_sprite, double area_factor,
  bear::universe::position_type gap )
{
  m_interactive_item = 
    new interactive_item
    ( &item, factor_sprite, area_factor, gap );
  
  m_interactive_item->set_z_position( item.get_z_position() - 10 );
  m_interactive_item->set_size( item.get_size() );
  m_interactive_item->set_center_of_mass( item.get_center_of_mass() );
  m_interactive_item->set_global( item.is_global() );
  
  item.new_item(*m_interactive_item);

  if ( !m_help.is_valid() )
    return;

  bear::decorative_item* const help = new bear::decorative_item();
  help->set_animation( m_help );
  help->set_size( m_help.get_max_size() );
  help->set_z_position( item.get_z_position() + 100 );

  item.new_item( *help );
  item.kill_when_dying( *help );

  bear::universe::forced_tracking mvt( bear::universe::position_type(0, 0) );
  mvt.set_reference_point
    ( bear::universe::ratio_reference_point
      ( item, bear::universe::position_type( 0, 0.5 ) ) );
  mvt.set_moving_item_ratio( bear::universe::position_type(0, 1) );

  help->set_forced_movement( mvt );
} // entity::create_interactive_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Return the corresponding interactive item.
 */
rp::interactive_item* rp::entity::get_interactive_item()
{
  return m_interactive_item;
} // entity::get_interactive_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Kill the corresponding interactive item.
 */
void rp::entity::kill_interactive_item()
{
  if ( m_interactive_item != NULL )
    {
      m_interactive_item->kill();
      m_interactive_item = NULL;
    }
} // rp::entity::kill_interactive_item()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the value of the combo.
 */
void rp::entity::set_combo_value(unsigned int value)
{
  m_combo_value = value;
} // rp::entity::set_combo_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update combo value after a collision.
 */
void rp::entity::update_combo_value(entity* item)
{
  if ( ( m_combo_value != 0 ) || ( item->get_combo_value() != 0 ) )
    {
      if ( m_combo_value > item->get_combo_value() )
        item->set_combo_value(m_combo_value + 1);
      else if ( m_combo_value < item->get_combo_value() )
        m_combo_value = item->get_combo_value() + 1;
      else
        {
          m_combo_value = m_combo_value + 1;
          item->set_combo_value(m_combo_value);
        }
    }
} // rp::entity::update_combo_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the value of the combo.
 */
unsigned int rp::entity::get_combo_value() const
{
  return m_combo_value;
} // rp::entity::get_combo_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set if the entity is passive.
 */
void rp::entity::set_passive(bool value)
{
  m_passive = value;
} // rp::entity::set_passive()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get if the entity is passive.
 */
bool rp::entity::get_passive() const
{
  return m_passive;
} // rp::entity::get_passive()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the value of the combo.
 */
std::string rp::entity::give_note()
{
  std::string note;
  
  if ( s_combo_note == 0 )
    note = "sound/C.ogg";
  else if ( s_combo_note == 1 )
    note = "sound/F.ogg";
  else if ( s_combo_note == 2 )
    note = "sound/D.ogg";
  else if ( s_combo_note == 3 )
    note = "sound/A.ogg";
  else if ( s_combo_note == 4 )
    note = "sound/E.ogg";
  else if ( s_combo_note == 5 )
    note = "sound/C.ogg";
  else 
    note = "sound/G.ogg";

  s_combo_note = ( s_combo_note + 1 ) % 7;

  return note;
} // rp::entity::get_combo_value()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets the animation showing hot to shoot the entity.
 * \param anim The animation.
 */
void rp::entity::set_help_animation( const bear::visual::animation& anim )
{
  m_help = anim;
} // entity::set_help_animation()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::entity::populate_loader_map( bear::engine::item_loader_map& m )
{
  m.insert( loader( *this ) );
} // entity::populate_loader_map()
