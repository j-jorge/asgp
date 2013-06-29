/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the balloon class.
 * \author Sebastien Angibaud
 */
#include "rp/balloon.hpp"
#include "rp/cable.hpp"
#include "rp/cannonball.hpp"
#include "rp/cart.hpp"
#include "rp/explosion.hpp"
#include "rp/game_variables.hpp"
#include "rp/plank.hpp"
#include "rp/tar.hpp"

#include "generic_items/decorative_item.hpp"
#include "generic_items/decorative_effect.hpp"

#include <boost/bind.hpp>

#include <claw/tween/easing/easing_linear.hpp>

BASE_ITEM_EXPORT( balloon, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::balloon::balloon()
: m_hit(false), m_fly(false), m_cart(NULL)
{
  set_phantom(true);
  set_mass(0.1);
  set_density(0.001);
} // rp::balloon::balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::balloon::pre_cache()
{
  super::pre_cache();
  
  get_level_globals().load_model("model/balloon.cm");
  get_level_globals().load_animation( "animation/balloon.canim" );
} // rp::balloon::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::balloon::on_enters_layer()
{
  super::on_enters_layer();

  set_model_actor( get_level_globals().get_model("model/balloon.cm") );
  start_model_action("idle");

  choose_color();
  create_interactive_item(*this, 1, 0);
  
  bear::engine::model_mark_placement mark;
      
  if ( get_mark_placement( "body", mark ) )
    {
      if ( get_passive() )
        {
          bear::visual::animation* anim = 
            new bear::visual::animation
            ( get_level_globals().get_animation
              ( "animation/balloon-slow.canim" ) );
          anim->set_time_factor( (double)rand() / RAND_MAX / 2.0 + 1.0 );
          set_global_substitute( "body", anim);
        }
      else
        {
          bear::visual::animation* anim = 
            new bear::visual::animation
            ( get_level_globals().get_animation( "animation/balloon.canim" ) );
          anim->set_time_factor( (double)rand() / RAND_MAX / 2.0 + 1.0 );
          set_global_substitute( "body", anim);
        }
    }
} // rp::balloon::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::balloon::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  if ( m_fly )
    m_tweener_y_position.update(elapsed_time);
} // rp::balloon::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the type of the attracted item.
 */
std::string rp::balloon::get_type() const
{
  return "balloon";
} // balloon::get_type()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when cart takes the object.
 * \param c The cart that take the object
 */
void rp::balloon::be_taken(cart* c)
{
  super::be_taken(c);

  kill();
} // balloon::be_taken()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::balloon::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  if ( ! collision_with_balloon(that,info) )
    if ( ! collision_with_cannonball(that) )
      if ( ! collision_with_cable(that) )
        if ( ! collision_with_plank(that) )
          if ( ! collision_with_tar(that) )
            collision_with_explosion(that,info);
} // balloon::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the explosition start.
 * \param counted Indicates if the balloon is counted in bad balloon count.
 */
void rp::balloon::explose(bool counted)
{ 
  set_transportability(false);
  kill_interactive_item();
  m_hit = true;
  m_fly = false;
  start_model_action("explose");
  create_decorations();
  if ( counted )
    game_variables::set_bad_balloon_number
      ( game_variables::get_bad_balloon_number() + 1);

  if ( get_attracted_state() )
    leave();
} // balloon::explose()

/*----------------------------------------------------------------------------*/
/**
 * \brief Indicates if the item can be attractable.
 */
bool rp::balloon::is_attractable() const
{
  return ( get_current_action_name() == "idle") &&
    super::is_attractable();
} // balloon::is_attractable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform that the item is attracted by the plunger.
 * \param p The plunger that attracts.
 */
void rp::balloon::attract(plunger* p)
{
  super::attract(p);

  m_fly = false;
} // balloon::attract()

/*----------------------------------------------------------------------------*/
/**
 * \brief The balloon fly.
 */
void rp::balloon::fly(cart* c)
{
  m_fly = true;
  m_cart = c;
  m_initial_position.x = 
    m_cart->get_horizontal_middle() - get_horizontal_middle();
  m_initial_position.y = get_vertical_middle();

  m_tweener_y_position =
    claw::tween::single_tweener
    ( 0.0, 2000.0, 13.0, 
      boost::bind( &rp::balloon::on_y_position_change, this, _1 ), 
      &claw::tween::easing_linear::ease_out );
} // balloon::fly()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the entity has a attack point.
 */
bool rp::balloon::has_attack_point() const
{
  return ! has_forced_movement();
} // rp::balloon::has_attack_point()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the attack point.
 */
bear::universe::position_type rp::balloon::get_attack_point() const
{
  return get_center_of_mass();
} // rp::balloon::get_attack_point()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cannonball.
 */
bool rp::balloon::collision_with_cannonball( bear::engine::base_item& that )
{ 
  bool result = false;
  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ! m_hit )
        {
          bool hit = true;
          
          if ( c->get_cart() != NULL && get_attracted_state() )
            { 
              bear::engine::model_mark_placement plunger_mark;      
              if ( c->get_cart()->get_mark_placement("plunger", plunger_mark) )
                hit = plunger_mark.get_position().distance
                  ( c->get_center_of_mass() ) > 200 ;
            }

          if ( hit )
            {
              explose();
              c->kill();
            }
        }
      
      result = true;
    }
  
  return result;
} // balloon::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonbal.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a cable.
 */
bool rp::balloon::collision_with_cable
( bear::engine::base_item& that )
{ 
  bool result = false;
  cable* c = dynamic_cast<cable*>(&that);
  
  if ( c != NULL ) 
    {
      if ( ! m_hit && ( c->is_ejected() ) )
        {
          if ( c->get_combo_value() != 0 )
            set_combo_value(c->get_combo_value()+1);          
          explose();
        }
      result = true;
    }
  
  return result;
} // balloon::collision_with_cable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \return Return True if the collision is with an explosion.
 */
bool rp::balloon::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( e != NULL ) 
    {
      if ( ! m_hit && e->test_in_explosion(info) )
        explose();
      
      result = true;
    }
  
  return result;
} // balloon::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a balloon.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a balloon.
 */
bool rp::balloon::collision_with_balloon
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  balloon* b = dynamic_cast<balloon*>(&that);
  
  if ( b != NULL ) 
    {
      default_collision(info); 
      result = true;
    }
  
  return result;
} // balloon::collision_with_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a plank.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a plank.
 */
bool rp::balloon::collision_with_plank( bear::engine::base_item& that )
{ 
  bool result = false;
  plank* p = dynamic_cast<plank*>(&that);
  
  if ( p != NULL ) 
    {
      if ( ! m_hit )
        explose();

      result = true;
    }
  
  return result;
} // balloon::collision_with_plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with tar.
 * \param that The other item of the collision.
 * \return Return True if the collision is with tar.
 */
bool rp::balloon::collision_with_tar( bear::engine::base_item& that )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {
      if ( ! m_hit && t->get_current_action_name() != "on_rail" )
        {
          create_tar_balloon();
          t->kill();
          if ( get_attracted_state() )
            leave();
          kill();
        }
            
      result = true;
    }
  
  return result;
} // balloon::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the explosition is finished.
 */
void rp::balloon::finish_explose()
{
   kill();
} // balloon::finish_explose()


/*----------------------------------------------------------------------------*/
/**
 * \brief Create all decorations when the balloon blasts.
 */
void rp::balloon::create_decorations()
{
  unsigned int a = (unsigned int)(6.0 * rand() / RAND_MAX);
  int b = (int)(2.0 * rand() / RAND_MAX);
  if ( b == 0 )
    b = -1;

  std::vector<bear::universe::speed_type> speeds(6);

  speeds[0] = bear::universe::speed_type(500*b, 0);
  speeds[1] = bear::universe::speed_type(500*b, 500);
  speeds[2] = bear::universe::speed_type(-500*b, 300);
  speeds[3] = bear::universe::speed_type(-500*b, 700);
  speeds[4] = bear::universe::speed_type(100*b, 1000);
  speeds[5] = bear::universe::speed_type(-100*b, 800);

  create_decorative_blast("balloon piece 1", speeds[a]);
  create_decorative_blast("balloon piece 2", speeds[(a+1)%6]);
  create_decorative_blast("balloon piece 3", speeds[(a+2)%6]);

  unsigned int nb = (unsigned int)(4.0 * rand() / RAND_MAX);
  if ( nb > 1 ) 
    {
      create_decorative_blast("balloon piece 4", speeds[(a+3)%6]);
      if ( nb > 2 ) 
        {
          create_decorative_blast("balloon piece 5", speeds[(a+4)%6]);
          if ( nb > 3 ) 
            create_decorative_blast("balloon piece 6", speeds[(a+5)%6]);
        }
    }
} // balloon::create_decorations

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a decorative balloon with tar.
 */
void rp::balloon::create_tar_balloon()
{
  bear::decorative_item* item = new bear::decorative_item;

  item->set_animation
    ( get_level_globals().auto_sprite
      ("gfx/balloon/balloon.png", "balloon 0") );
  item->auto_size();
  item->get_rendering_attributes().set_intensity(0,0,0);
  item->set_z_position(get_z_position());
  item->set_mass(0.1);
  item->set_density(0.002);
  item->set_artificial(true);
  item->set_phantom(true);
  item->set_kill_when_leaving(true);
  item->set_center_of_mass(get_center_of_mass());

  new_item( *item );

  CLAW_ASSERT( item->is_valid(),
         "The decoration of balloon isn't correctly initialized" );
} // balloon::create_tar_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a decoration when the balloon blasts.
 * \param sprite_name The name of the sprite in spritepos file.
 * \param speed The speed to give at the decoration.
 */
void rp::balloon::create_decorative_blast
(const std::string& sprite_name, const bear::universe::speed_type& speed)
{
  bear::decorative_item* item = new bear::decorative_item;

  item->set_animation
    ( get_level_globals().auto_sprite
      ("gfx/balloon/balloon.png", sprite_name) );
  item->get_rendering_attributes().combine(this->get_rendering_attributes());
  item->set_z_position(get_z_position() - 1);
  item->set_speed(speed);
  item->set_mass(10);
  item->set_density(2);
  item->set_artificial(true);
  item->set_phantom(true);
  item->set_kill_when_leaving(true);
  item->set_center_of_mass(get_center_of_mass());

  new_item( *item );

  CLAW_ASSERT( item->is_valid(),
         "The decoration of balloon isn't correctly initialized" );
} // balloon::create_decorative_blast()

/*----------------------------------------------------------------------------*/
/**
 * \brief Choose a random color.
 */
void rp::balloon::choose_color()
{
  switch( rand() % 8 )
    {
    case 0:
      get_rendering_attributes().set_intensity(1, 1, 0);
      break;
    case 1:
      get_rendering_attributes().set_intensity(0.4, 1, 0.13);
      break;
    case 2:
      get_rendering_attributes().set_intensity(0.85, 0.21, 1);
      break;
    case 3:
      get_rendering_attributes().set_intensity(1, 0.21, 0.42);
      break;
    case 4:
      get_rendering_attributes().set_intensity(0.95, 0.12, 0.12);
      break;
    case 5:
      get_rendering_attributes().set_intensity(1, 0.78, 0);
      break;
    case 6:
        get_rendering_attributes().set_intensity(0.12, 0.17, 0.95);
      break;
    case 7:
      get_rendering_attributes().set_intensity(0, 0.83, 0.58);
      break;
    }
} // balloon::choose_color()

/*----------------------------------------------------------------------------*/
/**
 * \brief Function called when the y-coordinate changes.
 * \param value The new y-coordinate.
 */
void rp::balloon::on_y_position_change
(const bear::universe::coordinate_type& value)
{
  set_center_of_mass
    ( m_cart->get_horizontal_middle() - m_initial_position.x,
      m_initial_position.y + value );
} // balloon::on_y_position_change

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::balloon::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( rp::balloon, finish_explose, void );
} // balloon::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::balloon )
