/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the explosion class.
 * \author Sebastien Angibaud
 */
#include "rp/explosion.hpp"

#include "rp/util.hpp" 
#include "rp/zeppelin.hpp" 

#include "universe/forced_movement/forced_tracking.hpp"
#include "generic_items/decorative_item.hpp"
#include "universe/collision_info.hpp"

BASE_ITEM_EXPORT( explosion, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::explosion::explosion()
:  m_nb_explosions(1), m_radius(10), m_duration(0)
{

} // rp::explosion::explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param nb_explosions Number of explosions to display.
 * \param radius The radius of the explosion.
 * \param decoration Indicates that explosion is a decoration.
 */
rp::explosion::explosion
( unsigned int nb_explosions, bear::universe::coordinate_type radius, 
  double duration, bool decoration )
  : m_nb_explosions(nb_explosions), m_radius(radius), m_duration(0),
    m_explosion_duration(duration)
{
  set_artificial(decoration);
} // rp::explosion::explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::explosion::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_animation("animation/explosion.canim");
  get_level_globals().load_animation("animation/effect/steam.canim");
} // rp::explosion::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::explosion::on_enters_layer()
{
  super::on_enters_layer();

  set_phantom(true);
  set_can_move_items(false);
  set_weak_collisions(true);

  create_explosion();
  create_smoke( 5 );
} // rp::explosion::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::explosion::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  const unsigned int nb_previous_explosions = 
    m_duration * m_nb_explosions / m_explosion_duration;

  m_duration += elapsed_time;
  const unsigned int nb_new_explosions = 
    (unsigned int)(m_duration * m_nb_explosions / m_explosion_duration) - 
    nb_previous_explosions; 

  const bear::universe::position_type center = get_center_of_mass();

  const bear::universe::coordinate_type height = 
    m_duration * ( 127.0 + 2.0 * m_radius ) / m_explosion_duration;
  set_size(height, height);
  set_center_of_mass(center);
  
  if (m_duration > m_explosion_duration + 1.0 ) 
    kill();
  else if (m_duration <= m_explosion_duration )
    for ( unsigned int i = 0; i != nb_new_explosions; ++i ) 
      {
        create_explosion();
        create_smoke();
      }
} // explosion::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::explosion::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  collision_with_zeppelin(that,info);
} // explosion::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Test if a collision is in explosion.
 * \param info Some informations about the collision.
 */
bool rp::explosion::test_in_explosion
(bear::universe::collision_info& info) const
{
  // TODO : check if item intersects circle
  return m_duration <= m_explosion_duration;
} // explosion::test_in_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with hot air balloon.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a hot air balloon.
 */
bool rp::explosion::collision_with_zeppelin
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  zeppelin* b = dynamic_cast<zeppelin*>(&that);
  
  if ( b != NULL ) 
    {
      if ( ! b->get_hit() && test_in_explosion(info) )
        {
          if ( get_combo_value() != 0 )
            b->set_combo_value(get_combo_value()+1);       
          b->explose();      
        }

      result = true;
    }
  
  return result;
} // explosion::collision_with_zeppelin()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create an explosion.
 */
void rp::explosion::create_explosion()
{
  bear::decorative_item* item = new bear::decorative_item;
  bear::universe::coordinate_type width = 64 + 64.0 * rand() / RAND_MAX;
    
  item->set_size(width,width);
  bear::visual::animation anim
    ( get_level_globals().get_animation("animation/explosion.canim") );
  anim.set_time_factor( 1 + 3 * (double)rand() / RAND_MAX);
  anim.set_size(width,width);
  
  item->set_animation(anim);
  item->set_z_position(get_z_position());
  item->set_artificial(true);
  item->set_phantom(true);
  item->set_kill_when_leaving(true);
  item->set_kill_when_finished(true);
  item->get_rendering_attributes().set_angle(6.29 * rand() / RAND_MAX);
  
  bear::universe::position_type pos =
    pick_random_position_in_annulus
    ( get_width() / std::sqrt(6.0), get_width() / 2 );
  
  item->set_center_of_mass(get_center_of_mass() + pos);
  
  new_item( *item );
  
  CLAW_ASSERT( item->is_valid(),
               "The decoration of explosion isn't correctly initialized" );

  bear::universe::forced_tracking m;
  m.set_reference_point_on_center(*this);
  m.set_distance(pos);
  item->set_forced_movement(m);
} // explosion::create_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create some smoke.
 * \param count The number of smoke clouds to create.
 */
void rp::explosion::create_smoke( std::size_t count ) const
{
  for ( std::size_t i(0); i!=count; ++i )
    create_smoke();
} // explosion::create_smoke()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create some smoke.
 */
void rp::explosion::create_smoke() const
{
  util::create_smoke( *this, 1, 0.6, 0.8, 0 );
} // explosion::create_smoke()

/*----------------------------------------------------------------------------*/
/**
 * \brief Finds a random position in an annulus.
 * \param r_min The inner radius of the annulus.
 * \param r_max The outer radius of the annulus.
 */
bear::universe::position_type
rp::explosion::pick_random_position_in_annulus
( bear::universe::coordinate_type r_min,
  bear::universe::coordinate_type r_max ) const
{
  const double a = 2 * 3.14159 * (double)rand() / RAND_MAX;
  const double r =
    std::sqrt( r_min + (r_max - r_min) * (double)rand() / RAND_MAX );

  return bear::universe::position_type( r * std::cos(a), r * std::sin(a) );
} // explosion::pick_random_position_in_annulus()
