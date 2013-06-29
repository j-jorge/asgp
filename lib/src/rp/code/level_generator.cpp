/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the target class.
 * \author Sebastien Angibaud
 */
#include "rp/level_generator.hpp"
#include "rp/hole.hpp"
#include "rp/target.hpp"
#include "rp/bonus.hpp"

BASE_ITEM_EXPORT( level_generator, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::level_generator::on_enters_layer()
{
  super::on_enters_layer();

  srand(time(NULL));
  create_level();
} // rp::level_generator::on_enters_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type item.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
  bool rp::level_generator::set_item_field
( const std::string& name, bear::engine::base_item* value )
{
  bool ok = true;

  if (name == "level_generator.right_hole")
    m_right_hole = straight_slope_handle(*value);
  else if (name == "level_generator.left_hole")
    m_left_hole = straight_slope_handle(*value);
  else
    ok = super::set_item_field(name, value);

  return ok;
} // level_generator::set_item_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type item_list.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
  bool rp::level_generator::set_item_list_field
( const std::string& name, const std::vector<bear::engine::base_item*>& value )
{
  bool ok = true;

  if (name == "level_generator.slopes")
    for( std::size_t i=0; i!=value.size(); ++i )
      m_slopes.push_back( slope_handle(value[i]) );
  else if (name == "level_generator.straight_slopes")
    for( std::size_t i=0; i!=value.size(); ++i )
      m_straight_slopes.push_back( straight_slope_handle(value[i]) );
  else if (name == "level_generator.obstacles")
    for( std::size_t i=0; i!=value.size(); ++i )
      m_obstacles.push_back( obstacle_handle(value[i]) );
  else 
    ok = super::set_item_list_field(name, value);

  return ok;
} // level_generator::set_item_list_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the level.
 */
void rp::level_generator::create_level()
{
  bear::universe::position_type pos(get_top_left());
  bool previous_is_slope(false);
  unsigned int last_paint = 0;

  for(unsigned int i = 0; i != 100; i++ )
    {
      const double selector = (double)rand() / RAND_MAX;

      if ( ( ( selector < 0.9 ) && previous_is_slope ) ||
	   ( ( selector < 0.5 ) && ! previous_is_slope ) )
	{
	  previous_is_slope = true;
	  add_slope(pos);
	}
      else
	{
	  previous_is_slope = false;
	  add_ground(pos);
	}

      create_target(pos);
      
      if ( ( ((double)rand() / RAND_MAX) < 0.1 ) && 
	   ( i - last_paint > 50 ) )
	{
	  last_paint = i;
	  create_bonus(pos);
	}
    }  
} // rp::level_generator::create_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a slope.
 */
void rp::level_generator::create_target(bear::universe::position_type& pos)
{
  target* new_target = new target();

  new_target->set_center_of_mass
    ( pos + 
      bear::universe::position_type
      ( (rand() % 600) - 200,
	(rand() % 400) - 200) );
  new_target->set_z_position(get_z_position()+10000);
  
  if ( ( (double)rand() / RAND_MAX ) < 0.5 ) 
    new_target->set_color("yellow");
  else
    new_target->set_color("red");

  new_item( *new_target );
} // level_generator::create_target()

/*----------------------------------------------------------------------------*/
/**
 * \brief create bonus.
 */
void rp::level_generator::create_bonus(bear::universe::position_type& pos)
{
  add_straight_slope(pos);

  bonus* new_bonus = new bonus();
  new_bonus->set_center_of_mass
    ( pos + bear::universe::position_type(0,100));
  
  new_bonus->set_z_position(get_z_position()+11000);
  
  new_item( *new_bonus );

  add_straight_slope(pos);
} // level_generator::create_bonus()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a slope.
 */
void rp::level_generator::add_slope(bear::universe::position_type& pos)
{
  unsigned int ind = rand() % m_slopes.size();

  bear::slope* new_slope;

  new_slope= new bear::slope( *(m_slopes[ind]) );
  
  if ( new_slope->get_steepness() < 0 ) 
    {
      new_slope->set_top_left(pos);
      new_item( *new_slope );
      
      pos.x += new_slope->get_width();
      pos.y += new_slope->get_steepness();
    }
  else
    {
      pos.y += new_slope->get_steepness();

      new_slope->set_top_left(pos);
      new_item( *new_slope );
      
      pos.x += new_slope->get_width();      
    }
} // rp::level_generator::add_slope()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a ground.
 */
void rp::level_generator::add_ground(bear::universe::position_type& pos)
{
  const double selector = (double)rand() / RAND_MAX;

  if ( selector < 0.5 )
    add_straight_slope(pos);
  else if ( selector < 0.9 )
    add_obstacle(pos);
  else 
    add_hole(pos);
} // level_generator::add_ground

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a straight_slope.
 */
void rp::level_generator::add_hole
(bear::universe::position_type& pos)
{
  add_straight_slope(pos);
  
  // left_hole
  bear::straight_slope* new_straight_slope_1;
  bear::universe::position_type p1(pos);
  p1.y += 5;
  new_straight_slope_1= new bear::straight_slope( *(m_left_hole) );
  new_item( *new_straight_slope_1 );
  new_straight_slope_1->set_top_left(p1);  
  pos.x += new_straight_slope_1->get_width();
  
  // create hole
  hole* new_hole = new hole();
  new_hole->set_size(400,50);
  new_hole->set_top_left(pos + bear::universe::position_type(-100,-50));
  new_item( *new_hole );
  
  pos.x += 200;
  
  // right_hole
  bear::straight_slope* new_straight_slope_2;
  bear::universe::position_type p2(pos);
  p2.y += 5;
  new_straight_slope_2 = new bear::straight_slope( *(m_right_hole) );
  new_item( *new_straight_slope_2 );
  new_straight_slope_2->set_top_left(p2);  
  pos.x += new_straight_slope_2->get_width();
  
  add_straight_slope(pos);
} // level_generator::add_hole()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add an obstacle.
 */
void rp::level_generator::add_obstacle(bear::universe::position_type& pos)
{
  add_straight_slope(pos);
  add_straight_slope(pos);

  unsigned int ind = rand() % m_obstacles.size();
  
  obstacle* new_obstacle = new obstacle(*(m_obstacles[ind]));
  new_obstacle->set_bottom_middle(pos);
  new_item( *new_obstacle );

  add_straight_slope(pos);
} // level_generator::add_obstacle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a straight_slope.
 */
void rp::level_generator::add_straight_slope
(bear::universe::position_type& pos)
{
  unsigned int ind = rand() % m_straight_slopes.size();

  bear::straight_slope* new_straight_slope;

  bear::universe::position_type p(pos);
  p.y += 5;

  new_straight_slope= new bear::straight_slope( *(m_straight_slopes[ind]) );

  new_item( *new_straight_slope );
  new_straight_slope->set_top_left(p);  
    
  pos.x += new_straight_slope->get_width();
} // rp::level_generator::add_straight_slope()

