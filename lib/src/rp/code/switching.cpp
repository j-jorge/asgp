/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the switching class.
 * \author Julien Jorge
 */
#include "rp/switching.hpp"
#include "rp/game_variables.hpp"
#include "rp/hole.hpp"
#include "rp/plunger.hpp"

BASE_ITEM_EXPORT( switching, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::switching::switching()
: m_initial_state("up"), m_steepness_up(76), m_z_up(-1000), m_up_slope(NULL),
  m_steepness_down(-76), m_z_down(0), m_down_slope(NULL), m_hole(NULL)
{

} // rp::switching::switching()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::switching::~switching()
{
  if ( (m_up_slope != NULL) && !m_up_slope->has_owner() )
    delete m_up_slope;

  if ( (m_down_slope != NULL) && !m_down_slope->has_owner() )
    delete m_down_slope;

  if ( (m_hole != NULL) && !m_hole->has_owner() )
    delete m_hole;
} // rp::switching::~switching()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::switching::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_model("model/switching.cm");
} // rp::switching::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the item is well initialized.
 */
bool rp::switching::is_valid()
{
  return (m_steepness_up != 0) && (m_steepness_down != 0) && super::is_valid();
} // rp::switching::is_valid()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the item.
 */
void rp::switching::on_enters_layer()
{
  super::on_enters_layer();

  set_model_actor( get_level_globals().get_model("model/switching.cm") );
  start_model_action(m_initial_state);
   
  bear::engine::model_mark_placement mark;

  if ( get_mark_placement("sign", mark) )
    create_interactive_item
        (*this, 0.25, 0, mark.get_position() - get_center_of_mass() );
} // rp::switching::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c string.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::switching::set_string_field
( const std::string& name, const std::string& value )
{
  bool result = true;

  if ( name == "switching.initial_state" )
    m_initial_state = value;
  else
    result = super::set_string_field(name, value);

  return result;
} // switching::set_string_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c real.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::switching::set_real_field( const std::string& name, double value )
{
  bool result = true;

  if ( name == "switching.up.steepness" )
    m_steepness_up = value;
  else if ( name == "switching.down.steepness" )
    m_steepness_down = value;
  else
    result = super::set_real_field(name, value);

  return result;
} // switching::set_real_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c int.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::switching::set_integer_field( const std::string& name, int value )
{
  bool result = true;

  if ( name == "switching.up.z" )
    m_z_up = value;
  else if ( name == "switching.down.z" )
    m_z_down = value;
  else
    result = super::set_integer_field(name, value);

  return result;
} // switching::set_integer_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the item tat he is entering the active region.
 */
void rp::switching::enters_active_region()
{
  super::enters_active_region();

  if ( m_initial_state != get_current_action_name() )
    {
      if ( get_current_action_name() == "up" && 
           m_initial_state == "down" )
        start_model_action("move down");
      else if ( get_current_action_name() == "down" && 
                m_initial_state == "up" )
        start_model_action("move up");
    }
} // switching::enters_active_region()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::switching::populate_loader_map( bear::engine::item_loader_map& m )
{
  super::populate_loader_map( m );

  m.insert( entity::loader( *this ) );
} // switching::populate_loader_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize a slope.
 * \param slope A reference to the pointer of the slope.
 * \param s The steepness.
 * \param z The z position of the slope.
 */
void rp::switching::init_slope( bear::slope*& slope, double s, int z )
{
  if ( slope == NULL )
    {
      slope = new bear::slope;

      if ( s < 0 )
        slope->set_size( get_width(), get_height() / 2 );
      else
        slope->set_size( get_size() );

      const double effective_steepness =
        get_rendering_attributes().is_mirrored() ? -s : s;

      slope->set_bottom_left( get_bottom_left() );
      slope->set_steepness( effective_steepness );
      slope->set_top_friction( 0.95 );
      slope->set_max_z_for_collision( z + 100 );
      slope->set_z_position(z);
      slope->set_z_shift(100);
      slope->set_force_z_position(true);

      new_item(*slope);
    }
  else
    get_layer().add_item(*slope);
} // rp::switching::init_slope()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start up state.
 */
void rp::switching::on_up()
{
  remove_hole_block();
  init_slope( m_up_slope, m_steepness_up, m_z_up );
} // rp::switching::on_up()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start down state.
 */
void rp::switching::on_down()
{
  remove_hole_block();
  init_slope( m_down_slope, m_steepness_down, m_z_down );
} // rp::switching::on_down()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start to move up.
 */
void rp::switching::on_move_up()
{
  CLAW_PRECOND( m_down_slope != NULL );

  m_down_slope->get_layer().drop_item( *m_down_slope );

  init_hole_block();
} // rp::switching::on_move_up()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start to move down.
 */
void rp::switching::on_move_down()
{
  CLAW_PRECOND( m_up_slope != NULL );

  m_up_slope->get_layer().drop_item( *m_up_slope );

  init_hole_block();
} // rp::switching::on_move_down()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the paint.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::switching::on_switch_down
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  on_switch(mark,that,info,"move down");
} // switching::on_switch_down()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the paint.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::switching::on_switch_up
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  on_switch(mark,that,info,"move up");
} // switching::on_switch_up()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with the paint.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \param action The next action.
 */
void rp::switching::on_switch
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info, const std::string & action )
{
  plunger* p = dynamic_cast<plunger*>(&that);
  
  if ( p != NULL )
    if ( ! p->come_back() ) 
      {
        game_variables::set_action_snapshot();
        p->stop(true);
        start_model_action(action);
      }
} // switching::on_switch()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add the hole block to kill the player if he collides while the switch
 *        moves.
 */
void rp::switching::init_hole_block()
{
  CLAW_PRECOND( m_hole == NULL );

  m_hole = new hole();
  m_hole->set_bounding_box(get_bounding_box());

  new_item(*m_hole);
} // switching::init_hole_block()

/*----------------------------------------------------------------------------*/
/**
 * \brief Remove the hole block.
 */
void rp::switching::remove_hole_block()
{
  if ( m_hole == NULL )
    return;

  m_hole->kill();
  m_hole = NULL;
} // switching::remove_hole_block()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::switching::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( switching, on_up, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( switching, on_down, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( switching, on_move_up, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( switching, on_move_down, void );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( switching, on_switch_up, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( switching, on_switch_down, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );
} // switching::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::switching )
