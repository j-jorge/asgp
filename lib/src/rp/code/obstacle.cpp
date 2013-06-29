/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the obstacle class.
 * \author Sebastien Angibaud
 */
#include "rp/obstacle.hpp"

#include "rp/cart.hpp"
#include "rp/plank.hpp" 
#include "rp/tar.hpp"
#include "rp/util.hpp"

#include "universe/collision_info.hpp"

BASE_ITEM_EXPORT( obstacle, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::obstacle::obstacle()
: m_random_explosion(false)
{
  set_can_move_items(true);
  set_phantom(false);
  set_artificial(false);
} // rp::obstacle::rp::obstacle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::obstacle::on_enters_layer()
{
  super::on_enters_layer();

  init(get_top_left());
  create_interactive_item(*this);
} // rp::obstacle::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do one step in the progression of the item.
 * \param elapsed_time Elasped time since the last progress.
 */
void rp::obstacle::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  update_items();
} // rp::obstacle::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Call collision_as_obstacle().
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::obstacle::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  if ( info.get_collision_side() == bear::universe::zone::top_zone )
    if ( default_collision(info) )
      collision_as_base_train(that, info);

  collision_with_tar(that);
} // obstacle::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type "boolean".
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::obstacle::set_bool_field
( const std::string& name, bool value )
{
  bool ok = true;

  if (name == "obstacle.random_explosion")
    m_random_explosion = value;
  else
    ok = super::set_bool_field(name, value);

  return ok;
} // obstacle::set_bool_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give a string representation of the item.
 * \param str (out) The result of the convertion.
 */
void rp::obstacle::to_string( std::string& str ) const
{
  super::to_string(str);
  base_train::to_string(str);
} // obstacle::to_string()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply the movement of the item.
 * \param elapsed_time Elasped time since the last call.
 */
void rp::obstacle::move( bear::universe::time_type elapsed_time )
{
  super::move(elapsed_time);

  update_item_positions( get_top_left(), get_speed() );
} // obstacle::move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create planks.
 * \param mark_name The prefix of mark name.
 */
void rp::obstacle::create_planks(const std::string& mark_name)
{
  if ( m_random_explosion )
    create_random_planks( mark_name );
  else
    create_fixed_planks( mark_name );
} // obstacle::create_planks()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adds the loaders of this item class into a given loader map.
 * \param m The map in which the loaders are inserted.
 */
void rp::obstacle::populate_loader_map( bear::engine::item_loader_map& m )
{
  super::populate_loader_map( m );

  m.insert( entity::loader( *this ) );
} // obstacle::populate_loader_map()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create random planks.
 * \param mark_name The prefix of mark name.
 */
void rp::obstacle::create_random_planks(const std::string& mark_name)
{
  for ( unsigned int i = 1; i <= 7; ++i )
    {
      std::ostringstream stream;
      stream << mark_name << " " << i;
      
      bear::universe::force_type force;
      force.x = -100000 + 200000 * (double)rand() / RAND_MAX;
      force.y = 100000 + 100000 * (double)rand() / RAND_MAX;

      create_plank(stream.str(),force);      
    }
} // obstacle::create_random_planks()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create fixed planks.
 * \param mark_name The prefix of mark name.
 */
void rp::obstacle::create_fixed_planks(const std::string& mark_name)
{
  std::vector< bear::universe::force_type > force;
  force.push_back(bear::universe::force_type( -100000,60000 ));
  force.push_back(bear::universe::force_type( -70000,100000 ));
  force.push_back(bear::universe::force_type( -30000,150000 ));
  force.push_back(bear::universe::force_type( 0,200000 ));
  force.push_back(bear::universe::force_type( 100000,60000 ));
  force.push_back(bear::universe::force_type( 70000,100000 ));
  force.push_back(bear::universe::force_type( 30000,150000 ));

  for ( unsigned int i = 1; i <= 7; ++i )
    {
      std::ostringstream stream;
      stream << mark_name << " " << i;
      create_plank(stream.str(),force[i-1]);      
    }
} // obstacle::create_fixed_planks()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the items concerned by a progress/move of this one.
 * \param d (out) A list to which are added such items.
 */
void rp::obstacle::get_dependent_items
( std::list<bear::universe::physical_item*>& d ) const
{
  super::get_dependent_items(d);

  get_items(d);
} // obstacle::get_dependent_items()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tar.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a tar.
 */
bool rp::obstacle::collision_with_tar( bear::engine::base_item& that )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {
      if ( t->get_current_action_name() != "on_rail" )
        {
          make_dirty();
          t->explose();
        }

      result = true;
    }
  
  return result;
} // obstacle::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the obstacle collides a tar.
 */
void rp::obstacle::make_dirty()
{  
  get_rendering_attributes().set_intensity(0,0,0);
} // obstacle::make_dirty()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a plank.
 * \param mark_name The name of the mark.
 * \param force The force to give.
 */
void rp::obstacle::create_plank
(const std::string& mark_name, const bear::universe::force_type& force)
{ 
  bear::engine::model_mark_placement mark;
  
  if ( get_mark_placement(mark_name, mark) )
    {
      plank* p = new plank(mark_name);
      p->set_combo_value(get_combo_value());
      p->set_z_position( mark.get_depth_position() );
      p->set_center_of_mass(mark.get_position());
      p->add_external_force(force);
      p->set_system_angle(mark.get_angle());
      p->get_rendering_attributes().combine(get_rendering_attributes());

      const double angular_speed = 1 + 20 * (double)rand() / RAND_MAX;

      if ( force.x < 0 )
        p->set_angular_speed( -angular_speed );
      else
        p->set_angular_speed( angular_speed );

      new_item(*p);
    }
} // obstacle::create_plank()
