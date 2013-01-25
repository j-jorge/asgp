/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the decorative_balloon class.
 * \author Sebastien Angibaud
 */
#include "rp/decorative_balloon.hpp"
#include "rp/cart.hpp"

#include "engine/scene_visual.hpp"
#include "visual/scene_line.hpp"
#include "visual/color.hpp"

#include <boost/bind.hpp>

#include <claw/tween/easing/easing_linear.hpp>

BASE_ITEM_EXPORT( decorative_balloon, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::decorative_balloon::decorative_balloon()
: m_cart(NULL), m_anchor_name("balloon_anchor"), m_fly(false), m_is_linked(true)
{
} // rp::decorative_balloon::decorative_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param c The cart.
 * \param anchor_name The name of the anchor for link.
 */
rp::decorative_balloon::decorative_balloon
(const cart* c, const std::string& anchor_name)
  : m_cart(c), m_anchor_name(anchor_name), m_fly(false), m_is_linked(true),
    m_gap_z(-2)
{
} // rp::decorative_balloon::decorative_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::decorative_balloon::on_enters_layer()
{
  super::on_enters_layer();

  set_size(34,36);
  set_gap(-4,-25);
  set_system_angle_as_visual_angle(true);
  set_phantom(true);
  set_can_move_items(true);
  set_artificial(true);
  set_density(0.0008);
  set_mass(0.1);
  m_initial_distance = bear::universe::position_type(0,0);
} // decorative_balloon::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::decorative_balloon::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  update_angle();

  if ( m_cart != NULL )
    set_z_position( m_cart->get_z_position() + m_gap_z ); 

  if ( m_fly )
    {
      if ( m_duration < m_waiting + 1 )
        {
          m_duration += elapsed_time;
          if ( m_duration >= m_waiting )
            {
              get_rendering_attributes().set_opacity(1);
              m_tweener_fly.clear();
              m_tweener_fly.insert
                ( claw::tween::single_tweener
                  ( 0.05, 1, 2,
                    boost::bind( &rp::decorative_balloon::on_distance_change,
                                 this, _1 ), 
                    &claw::tween::easing_linear::ease_out ) );
              m_tweener_fly.insert
                ( claw::tween::single_tweener
                  ( 1, 1, 1000,
                    boost::bind( &rp::decorative_balloon::on_distance_change,
                                 this, _1 ), 
                    &claw::tween::easing_linear::ease_out ) );
            }
        }
      
      m_tweener_fly.update(elapsed_time);

      bear::universe::coordinate_type horizontal_middle = 
        get_horizontal_middle();
      m_tweener_width.update(elapsed_time);
      m_tweener_height.update(elapsed_time);
      set_horizontal_middle(horizontal_middle);
    }
} // decorative_balloon::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the sprite representing the item.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::decorative_balloon::get_visual
( std::list<bear::engine::scene_visual>& visuals ) const
{
   super::get_visual(visuals);
   
   if ( ( m_cart != NULL ) && m_is_linked )
     {
       std::vector<bear::universe::position_type>v;
       v.push_back(m_cart->get_balloon_anchor_position(m_anchor_name));
       v.push_back(get_center_of_mass());
       
       bear::visual::color color(0,0,0);
       color.set_opacity(0.4 * get_rendering_attributes().get_opacity());
       bear::visual::scene_line scene =
         bear::visual::scene_line( 0, 0, color, v, 1 );
       visuals.push_front( bear::visual::scene_element( scene ) );
     }
} // decorative_balloon::get_visuals();

/*----------------------------------------------------------------------------*/
/**
 * \brief Choose a random color.
 */
void rp::decorative_balloon::choose_balloon_color()
{
  unsigned int r = rand()%8;
  
  if ( r == 0 )
    get_rendering_attributes().set_intensity(1, 1, 0);
  else if ( r == 1 )
    get_rendering_attributes().set_intensity(0.4, 1, 0.13);
  else if ( r == 2 )
    get_rendering_attributes().set_intensity(0.85, 0.21, 1);
  else if ( r == 3 )
    get_rendering_attributes().set_intensity(1, 0.21, 0.42);
  else if ( r == 4 )
    get_rendering_attributes().set_intensity(0.95, 0.12, 0.12);
  else if ( r == 5 )
    get_rendering_attributes().set_intensity(1, 0.78, 0);
  else if ( r == 6 )
    get_rendering_attributes().set_intensity(0.12, 0.17, 0.95);
  else
    get_rendering_attributes().set_intensity(0, 0.53, 0.08);
} // decorative_balloon::choose_color()

/*----------------------------------------------------------------------------*/
/**
 * \brief The balloon must fly.
 * \param waiting The duration before flying.
 * \param y_distance The distance on y-coordinate with anchor.
 * \param new_balloon Indicates if item is a new balloon.
 */
void rp::decorative_balloon::fly
(double waiting, double y_distance, bool new_balloon)
{
  m_fly = true;
  m_waiting = waiting;
  m_duration = 0;
  
  if ( new_balloon )
    {
      get_rendering_attributes().set_opacity(0);
      m_distance = bear::universe::position_type(0,y_distance);
      
      m_tweener_height.clear();
      m_tweener_height.insert
        ( claw::tween::single_tweener
         ( 1, 1, m_waiting,
           boost::bind( &rp::decorative_balloon::on_height_change, this, _1 ), 
           &claw::tween::easing_linear::ease_out ) );
      m_tweener_height.insert
        ( claw::tween::single_tweener
          ( 1, get_animation().height(), 1,
            boost::bind( &rp::decorative_balloon::on_height_change, this, _1 ), 
            &claw::tween::easing_linear::ease_in ) );
      
      m_tweener_width.clear();
      m_tweener_width.insert
        ( claw::tween::single_tweener
          ( 1, 1, m_waiting,
            boost::bind( &rp::decorative_balloon::on_width_change, this, _1 ), 
            &claw::tween::easing_linear::ease_out ) );
      m_tweener_width.insert
        ( claw::tween::single_tweener
          ( 1, get_animation().width(), 1,
            boost::bind( &rp::decorative_balloon::on_width_change, this, _1 ), 
            &claw::tween::easing_linear::ease_in ) );
    }
  else
    {
      remove_all_links();
      m_initial_distance = get_bottom_middle() -
        m_cart->get_balloon_anchor_position(m_anchor_name);

      m_distance = 
        bear::universe::position_type
        (0, m_initial_distance.distance(bear::universe::position_type(0,0))) -
        m_initial_distance;
    }

  m_tweener_fly.insert
    ( claw::tween::single_tweener
      ( 0, 0, m_waiting,
        boost::bind( &rp::decorative_balloon::on_distance_change,
                     this, _1 ), 
        &claw::tween::easing_linear::ease_out ) );
  m_tweener_fly.insert
    ( claw::tween::single_tweener
      ( 0, 0.05, 1,
        boost::bind( &rp::decorative_balloon::on_distance_change,
                     this, _1 ), 
        &claw::tween::easing_linear::ease_out ) );
} // decorative_balloon::fly()

/*----------------------------------------------------------------------------*/
/**
 * \brief The balloon is free.
 */
void rp::decorative_balloon::free()
{
  m_is_linked = false;
  set_density(0.00099);
  remove_all_links();
} // decorative_balloon::free()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the gap of z position.
 * \param The gap on z position with cart.
 */
void rp::decorative_balloon::set_gap_z( int gap_z )
{
  m_gap_z = gap_z;

  if ( m_cart != NULL )
    set_z_position( m_cart->get_z_position() + m_gap_z); 
} // decorative_balloon::set_gap_z()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update distance on y-coordinate.
 * \param value The distance with mark item.
 */
void rp::decorative_balloon::on_distance_change
(const bear::universe::coordinate_type& value)
{
  if ( m_cart != NULL )
    {
      bear::universe::position_type pos
        (m_cart->get_balloon_anchor_position(m_anchor_name));
      pos.x += value * m_distance.x + m_initial_distance.x;
      pos.y += value * m_distance.y + m_initial_distance.y;

      set_bottom_middle(pos);
    }
} // rp::decorative_balloon::on_distance_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update width of the balloon.
 * \param value The new width.
 */
void rp::decorative_balloon::on_width_change
(const bear::universe::coordinate_type& value)
{
  get_animation().set_width(value);
} // rp::decorative_balloon::on_width_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update height of the balloon.
 * \param value The new height.
 */
void rp::decorative_balloon::on_height_change
(const bear::universe::coordinate_type& value)
{
  get_animation().set_height(value);
} // rp::decorative_balloon::on_height_change()

/*---------------------------------------------------------------------------*/
/**
 * \brief Update angle of the item.
 */
void rp::decorative_balloon::update_angle()
{
  const bear::universe::vector_type vect
    (get_center_of_mass() - m_cart->get_balloon_anchor_position(m_anchor_name));
  
  set_system_angle(std::atan2( vect.y, vect.x ) - 1.57);
} // decorative_balloon::update_angle()
