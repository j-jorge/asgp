/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the cart class.
 * \author Sebastien Angibaud
 */
#include "rp/cart.hpp"

#include "rp/attractable_item.hpp"
#include "rp/balloon.hpp"
#include "rp/decorative_balloon.hpp"
#include "rp/bird.hpp"
#include "rp/bomb.hpp"
#include "rp/bonus.hpp"
#include "rp/cannonball.hpp"
#include "rp/cursor.hpp"
#include "rp/defines.hpp"
#include "rp/game_variables.hpp"
#include "rp/explosion.hpp"
#include "rp/hole.hpp"
#include "rp/level_exit.hpp"
#include "rp/obstacle.hpp"
#include "rp/plunger.hpp"
#include "rp/switching.hpp"
#include "rp/tar.hpp"
#include "rp/util.hpp"
#include "rp/transition_effect/level_ending_effect.hpp"
#include "rp/zeppelin.hpp"

#include "engine/level.hpp"
#include "engine/game.hpp"
#include "engine/message/transition_effect_message.hpp"
#include "engine/reference_point/model_mark_reference_point.hpp"
#include "engine/world.hpp"
#include "generic_items/camera_shaker.hpp"
#include "generic_items/decorative_effect.hpp"
#include "generic_items/delayed_kill_item.hpp"
#include "generic_items/delayed_level_loading.hpp"
#include "universe/forced_movement/forced_join.hpp"
#include "universe/forced_movement/forced_sequence.hpp"
#include "universe/forced_movement/center_of_mass_reference_point.hpp"
#include "universe/collision_info.hpp"
#include "universe/zone.hpp"
#include "universe/link/chain_link.hpp"
#include "engine/scene_visual.hpp"
#include "visual/scene_line.hpp"
#include "visual/color.hpp"
#include "visual/scene_polygon.hpp"

#include "generic_items/debug/runtime_setting.hpp"

#include <sstream>
#include <boost/bind.hpp>
#include <algorithm>

#include <claw/tween/easing/easing_cubic.hpp>

BASE_ITEM_EXPORT( cart, rp )

const bear::universe::time_type rp::cart::s_injured_duration = 1;
const bear::universe::time_type rp::cart::s_fire_duration = 0.5;
#if defined( __ANDROID__ )
const bear::universe::time_type rp::cart::s_smoke_delay = 0.60;
#else
const bear::universe::time_type rp::cart::s_smoke_delay = 0.15;
#endif

unsigned int rp::cart::s_score = 0;
const double rp::cart::s_min_cannon_angle = -0.25;
const double rp::cart::s_max_cannon_angle = 1.7;
const double rp::cart::s_min_arm_angle = -2.2;
const double rp::cart::s_max_arm_angle = 2.2;
const double rp::cart::s_min_speed_length = 150;
const unsigned int rp::cart::s_decorative_balloons_number = 5;

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::cart::cart()
  : m_progress(NULL), m_right_orientation(true), m_ground_force(100000,0),
    m_hot_spot_position(0, 0),
    m_hot_spot_target(0, 0),
    m_hot_spot_balance_move(0, 0), m_spot_item(NULL),
    m_cursor(NULL), m_on_switching(false),
    m_previous_bottom_contact(false), m_speed_on_contact(0, 0),
    m_previous_top_contact(false), 
    m_gap_mouse(0.5,0.5),
    m_force_factor(1), m_want_crouch(false), m_can_jump(true), 
    m_looping(false),
    m_maximum_plungers(3), m_link_balloon_visual(NULL), 
    m_arm_angle(0), m_good_fire(true), m_good_arm_angle(true),
    m_is_injured(false), m_level_timer(NULL),
    m_injured_duration(0), m_fire_duration(s_fire_duration),
    m_next_smoke(0), m_passive(false), m_id(1),
    m_bad_plunger_zone_rendering(true), m_combo_sample(NULL)
{
  set_system_angle_as_visual_angle(true);
  set_phantom(false);
  set_can_move_items(false);
  set_artificial(false);
  set_global(true);
  set_z_fixed(false);
  set_mass(100);
} // cart::cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a cart from an other cart.
 * \param that The cart from which the new one is built.
 */
rp::cart::cart( const cart& that )
{
  CLAW_FAIL("rp::cart cannot be copied.");
} // cart::cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::cart::~cart()
{
  for ( ; !m_signals.empty(); m_signals.pop_front() )
    m_signals.front().disconnect();

  delete m_combo_sample;
} // cart::~cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Loads the media required by this class.
 */
void rp::cart::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_model("model/cart.cm");

  get_level_globals().load_sound("sound/empty.ogg");
  get_level_globals().load_sound("sound/effect/tiny-bell.ogg");
  get_level_globals().load_sound("sound/cart/empty-cannon.ogg");
  get_level_globals().load_sound("sound/hit.ogg");
  get_level_globals().load_sound("sound/metal-1.ogg");
  get_level_globals().load_sound("sound/metal-2.ogg");
  get_level_globals().load_sound("sound/metal-3.ogg");
  get_level_globals().load_sound("sound/metal-4.ogg");
  get_level_globals().load_sound("sound/shot.ogg");
  get_level_globals().load_sound("sound/cart/cannon.ogg");
  get_level_globals().load_sound("sound/cart/crash.ogg");
  get_level_globals().load_sound("sound/cart/hit.ogg");
  get_level_globals().load_sound("sound/cart/andy/jump.ogg");
  get_level_globals().load_sound("sound/cart/andy/sad.ogg");  
  get_level_globals().load_sound("sound/cart/andy/ah-ah.ogg");
  get_level_globals().load_sound("sound/cart/andy/cool.ogg");
  get_level_globals().load_sound("sound/cart/andy/yeah-1.ogg");
  get_level_globals().load_sound("sound/cart/andy/yeah-2.ogg");
  get_level_globals().load_sound("sound/cart/andy/yeah-3.ogg");
  get_level_globals().load_sound("sound/cart/andy/whoo.ogg");
  get_level_globals().load_sound("sound/cart/andy/whohoo.ogg");
  get_level_globals().load_sound("sound/cart/andy/whoooooo.ogg");
  get_level_globals().load_sound("sound/medal/bronze.ogg");
  get_level_globals().load_sound("sound/medal/silver.ogg");
  get_level_globals().load_sound("sound/medal/gold.ogg");
  get_level_globals().load_sound("sound/medal/applause.ogg");

  get_level_globals().load_sound("music/cart/dead.ogg");
  get_level_globals().load_sound("music/cart/lose.ogg");
  get_level_globals().load_sound("music/cart/win.ogg");

  get_level_globals().load_image("gfx/cart/cart.png");
  get_level_globals().load_image("gfx/status/cursor.png");
  get_level_globals().load_image("gfx/status/status.png");
  get_level_globals().load_image("gfx/status/score.png");
  get_level_globals().load_image("gfx/common.png");
  get_level_globals().load_image("gfx/common.png");
  get_level_globals().load_image( get_theme_image_name() );

  get_level_globals().load_animation("animation/balloon-slow.canim");
  get_level_globals().load_animation("animation/effect/wave.canim");
  get_level_globals().load_animation("animation/effect/double-wave.canim");
  get_level_globals().load_animation("animation/effect/steam.canim");
  get_level_globals().load_animation("animation/fire.canim");

  // fireworks
  get_level_globals().load_sound( "sound/fireworks/explosion-1.ogg");
  get_level_globals().load_sound( "sound/fireworks/explosion-2.ogg");
  get_level_globals().load_sound( "sound/fireworks/explosion-3.ogg");
  get_level_globals().load_sound( "sound/fireworks/explosion-4.ogg");
  get_level_globals().load_sound( "sound/fireworks/whistling-1.ogg");
  get_level_globals().load_sound( "sound/fireworks/whistling-2.ogg");
} // cart::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::cart::on_enters_layer()
{
  super::on_enters_layer();

  init_signals();
  set_spot_target_position(500, 200);
  set_spot_balance_move(5, 5);
  m_hot_spot_position.x = 500;
  m_hot_spot_position.y = 200;
  
  set_elasticity(0.2);
  
  m_tweener_fire_angle = claw::tween::single_tweener
    ( 0, 0, 1,
      boost::bind
      ( &rp::cart::on_fire_angle_change,this, _1 ), 
      &claw::tween::easing_cubic::ease_out );
  
  m_progress = NULL;

  if ( m_id == 1 )
    set_model_actor( get_level_globals().get_model("model/cart.cm") );
  else
    set_model_actor( get_level_globals().get_model("model/cart-brian.cm") );

  start_model_action("move");
  save_position();
  init_elements(); 
  create_cursor();
} // cart::on_enters_layer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::cart::progress( bear::universe::time_type elapsed_time )
{
  if ( ! game_variables::is_level_ending() ) 
    progress_spot( elapsed_time );

  super::progress( elapsed_time );

  if ( !m_passive )
    m_cursor->set_center_of_mass
    ( bear::universe::position_type
      ( get_level().get_camera_focus().left() + 
        m_gap_mouse.x * get_level().get_camera_focus().width(),
        get_level().get_camera_focus().bottom() + 
        m_gap_mouse.y * get_level().get_camera_focus().height() ) );

  if ( game_variables::level_has_started()
       && ( !game_variables::is_boss_transition() || m_passive ) )
    { 
      progress_tweeners( elapsed_time );
      progress_injured_state( elapsed_time );
      if ( m_progress != NULL )
        (this->*m_progress)(elapsed_time);
      
      if ( get_current_action_name() != "with_tar" )
        {
          progress_arm_angle();
          progress_cannon();
          progress_plunger();
          progress_fire();
          set_mark_position_in_action("gun", compute_gun_position());
        }
    }

  if ( !m_passive )
    progress_input_reader(elapsed_time);      

  if ( game_variables::level_has_started() )
    { 
      m_can_jump = true;
      m_force_factor = 1;
      m_fire_duration += elapsed_time;
      update_status_informations();
      update_bottom_contact();
    }

  if ( has_bottom_contact() && get_bottom_contact().get_max() < 1.0 &&
       game_variables::is_level_ending() && game_variables::is_boss_level() )
    apply_impulse_jump();

  if ( !can_finish() )
    create_smoke( elapsed_time );
} // cart::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the sprite representing the item.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void
rp::cart::get_visual( std::list<bear::engine::scene_visual>& visuals ) const
{
   super::get_visual(visuals);

   #if 0
   add_plunger_link(visuals);
   #endif
   add_bad_plunger_zone(visuals);
} // cart::get_visuals();

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c boolean.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool rp::cart::set_bool_field( const std::string& name, bool value )
{
  bool result(true);

  if ( name == "cart.bad_plunger_zone_rendering" )
    m_bad_plunger_zone_rendering = value;
  else
    result = super::set_bool_field(name, value);

  return result;
} // cart::set_bool_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c unsigned int.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::cart::set_u_integer_field
( const std::string& name, unsigned int value )
{
  bool result = true;

  if (name == "cart.id")
    m_id = value;  
  else
    result = super::set_u_integer_field(name, value);

  return result;
} // cart::set_u_integer_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c base_item.
 * \param name The name of the field.
 * \param value The new value of the field.
 * \return false if the field "name" is unknow, true otherwise.
 */
bool rp::cart::set_item_field
( const std::string& name, bear::engine::base_item* value )
{
  bool result = true;

  if ( name == "cart.spot_item" )
    m_spot_item = value;
  else if ( name == "cart.cursor" )
    m_cursor = dynamic_cast<cursor*>( value );
  else
    result = super::set_item_field(name,value);

  return result;
} // cart::set_item_field()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a star behing the cart.
 * \param ref The item around which the star is created.
 */
void rp::cart::create_hit_star( bear::engine::base_item& ref ) const
{
  util::create_hit_star
    ( ref, bear::visual::color_type("#e0e0e0"),
      bear::visual::color_type("#db2200"), s_injured_duration );
} // cart::create_hit_star()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a explosion on the cannon.
 */
void rp::cart::create_cannon_fire()
{
  bear::engine::model_mark_placement mark_fire;
  
  if ( get_mark_placement("fire", mark_fire) )
    set_global_substitute
      ("fire", new bear::visual::animation
       (get_level_globals().get_animation
        ("animation/fire.canim") ) );
} // cart::create_cannon_fire()

/*----------------------------------------------------------------------------*/
/**
 * \brief Regenerate the cart.
 */
void rp::cart::regenerate()
{ 
  bear::delayed_level_loading* item = new bear::delayed_level_loading
        ( get_level().get_filename(), 2, false, 2, 
          RP_TRANSITION_EFFECT_DEFAULT_TARGET_NAME);
  new_item(*item);
  item->set_global(true);
  item->set_center_of_mass(get_center_of_mass());
  util::send_die_level(get_level().get_filename());
} // cart::regenerate()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform that a plunger is back.
 * \param p The plunger.
 * \param attracted_item The item that is attracted.
 */
void rp::cart::finish_plunger
( plunger* p, plunger::handle_type& attracted_item)
{
  if ( attracted_item != plunger::handle_type(NULL) )
    {
      if ( get_current_action_name() != "takeoff" && 
           get_current_action_name() != "dead")
        {
          if ( attracted_item->get_type() == "balloon" )
            add_balloon(attracted_item.get());
          else if ( attracted_item->get_type() == "bomb" )
            is_hit();
        
          attracted_item->be_taken(this);
        }  
    }

  m_plungers.erase(p);
} // cart::finish_plunger()

/*---------------------------------------------------------------------------*/
/**
 * \brief Function called when cart is hit.
 */
void rp::cart::is_hit()
{
  if ( ! m_is_injured && ! game_variables::is_level_ending() )
    {
      m_is_injured = true;
      m_injured_duration = 0;

      unsigned int lives =
        game_variables::get_cart_elements_number(m_id);

      if ( lives > 1 )
        {
          create_hit_star(*this);
          lose_balloons(1);

          get_level_globals().play_sound
            ( "sound/hit.ogg",
              bear::audio::sound_effect( get_center_of_mass() ) );
          get_level_globals().play_sound
            ( "sound/cart/hit.ogg",
              bear::audio::sound_effect( get_center_of_mass() ) );

          bear::camera_shaker::shake_around
            ( *this, get_level().get_camera_focus().height() / 36, 0.3 );
          
          if ( lives > 2 )
            lose_elements();

          game_variables::set_cart_elements_number
            ( m_id, game_variables::get_cart_elements_number(m_id) - 1 );
        }
      else
        die(false, false);
    }
} // cart::is_hit()

/*----------------------------------------------------------------------------*/
/**
 * \brief The cart die by a wall.
 */
void rp::cart::die_by_wall()
{
  if ( ! game_variables::is_level_ending() )
    {
      create_hit_star( *this );
      
      die(false, false);
    }
} // cart::die_by_wall()

/*----------------------------------------------------------------------------*/
/**
 * \brief The cart die.
 * \param right Indicates if the cart is hit on the right side.
 * \param left Indicates if the cart is hit on the right side.
 */
void rp::cart::die( bool right, bool left )
{
  while( ! m_plungers.empty() )
    {
      plunger * p = * (m_plungers.begin());
      plunger::handle_type handle(NULL);
      finish_plunger(p, handle);
      p->kill();
    }
  
  set_phantom(true);
  
  start_model_action("dead");
  game_variables::set_level_ending(true);
  
  if ( right ) 	     
    add_external_force(bear::universe::force_type(-3000000,3000000));
  else if ( left ) 
    add_external_force(bear::universe::force_type(4000000,5000000));

  get_level_globals().stop_all_musics( 0.5 );
  get_level_globals().play_sound( "music/cart/dead.ogg" );

  get_level_globals().play_sound
    ( "sound/cart/crash.ogg", bear::audio::sound_effect(get_center_of_mass()) );
} // cart::die()

/*----------------------------------------------------------------------------*/
/**
 * \brief The cart jumps.
 */
void rp::cart::jump()
{
  if ( get_current_action_name() != "dead" )
    {
      bear::universe::speed_type speed(get_speed());
      speed.y = 0;
      set_speed(speed);

      apply_impulse_jump();
    }
} // cart::jump()

/*---------------------------------------------------------------------------*/
/**
 * \brief Set the right orientation.
 * \param right_orientation The new right orientation.
 */
void rp::cart::set_right_orientation(bool right_orientation)
{
  m_right_orientation = right_orientation;
} // cart::set_right_orientation()

/*---------------------------------------------------------------------------*/
/**
 * \brief Set the force given on the ground.
 * \param ground_force The new ground force.
 */
void rp::cart::set_ground_force
( const bear::universe::force_type& ground_force)
{ 
  m_ground_force = ground_force;
} // cart::set_ground_force()

/*---------------------------------------------------------------------------*/
/**
 * \brief Set the force given on the ground.
 * \param force_factor The new ground force.
 */
void rp::cart::set_force_factor
(const bear::universe::coordinate_type& force_factor )
{ 
  m_force_factor = force_factor;
} // cart::set_force_factor()

/*---------------------------------------------------------------------------*/
/**
 * \brief Set if cart can jump.
 * \param value The new jump status.
 */
void rp::cart::set_can_jump(bool can_jump )
{ 
  m_can_jump = can_jump;
} // cart::set_can_jump()

/*---------------------------------------------------------------------------*/
/**
 * \brief Set if cart is in a top looping.
 * \param value The new looping status.
 
 */
void rp::cart::set_looping(bool looping )
{
  m_looping = looping;
} // cart::set_looping()

/*---------------------------------------------------------------------------*/
/**
 * \brief The cart starts takeoff.
 */
void rp::cart::apply_takeoff()
{
  if ( ( get_current_action_name() != "dead" ) && 
       ( get_current_action_name() != "takeoff" ) )
    {
      if ( game_variables::is_boss_level() )
        game_variables::set_balloons_number(20);
      m_progress = &rp::cart::progress_takeoff;
      start_model_action("takeoff");
    } 
} // cart::apply_takeoff()

/*---------------------------------------------------------------------------*/
/**
 * \brief Get the score.
 */
unsigned int rp::cart::get_score() const
{
  return s_score;
} // cart:get_score()

/*---------------------------------------------------------------------------*/
/**
 * \brief Tell the cart to ignore the input from the player.
 */
void rp::cart::set_passive()
{
  m_passive = true;
} // cart:set_passive()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the position.
 */
void rp::cart::save_position()
{
  m_origin = get_bottom_middle() + bear::universe::position_type(0,5);
  m_origin_z = get_z_position();
} // cart::save_position()

/*---------------------------------------------------------------------------*/
/**
 * \brief Get position of plunger.
 */
bear::universe::position_type rp::cart::get_plunger_position() const
{
  bear::engine::model_mark_placement arm_mark;

  if ( get_current_local_mark_placement("arm", arm_mark) )
    {
      bear::universe::position_type pos = arm_mark.get_position();

      pos.x += 55 * cos(m_arm_angle) - 2 * sin(m_arm_angle);
      pos.y += 55 * sin(m_arm_angle) + 2 * cos(m_arm_angle);
      
      return pos;
    }
  else
    return bear::universe::position_type();
} // cart::get_plunger_position()

/*---------------------------------------------------------------------------*/
/**
 * \brief Get position of balloon anchor.
 * \param anchor_name The name of the anchor.
 */
bear::universe::position_type 
rp::cart::get_balloon_anchor_position(const std::string& anchor_name) const
{
  bear::engine::model_mark_placement mark;

  if ( get_mark_placement(anchor_name, mark) )
    return mark.get_position();
  else
    return get_center_of_mass();
} // cart::get_balloon_anchor_position()

/*---------------------------------------------------------------------------*/
/**
 * \brief Indicates if the level can be finish.
 */
bool rp::cart::can_finish() const
{
  return ! get_level().get_camera_focus().intersects( get_bounding_box() );
} // cart::can_finish()

/*---------------------------------------------------------------------------*/
/**
 * \brief Give an element.
 */
void rp::cart::give_element()
{
  unsigned int lives = game_variables::get_cart_elements_number(m_id);
  if ( lives != 3 )
    {
      std::map<std::string, bool>::iterator it;
      for ( it = m_elements.begin(); it != m_elements.end(); ++it )
        {
          it->second = true;
          init_element(it->first);
        }

      game_variables::set_cart_elements_number(m_id,3);
    }
} // cart::give_element

/*---------------------------------------------------------------------------*/
/**
 * \brief Give plunger bonus.
 */
void rp::cart::give_plunger_bonus()
{
  game_variables::set_plunger_total_number
    ( 2 * game_variables::get_plunger_total_number() );
} // cart::give_plunger_bonus

/*---------------------------------------------------------------------------*/
/**
 * \brief Set the level timer.
 */
void rp::cart::set_level_timer(const bear::timer* level_timer)
{
  m_level_timer = level_timer;
} // cart::set_level_timer()

/*---------------------------------------------------------------------------*/
/**
 * \brief Get the level timer.
 */
const bear::timer* rp::cart::get_level_timer() const
{
  return m_level_timer;
} // cart::get_level_timer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the items concerned by a progress/move of this one.
 * \param d (out) A list to which are added such items.
 */
void rp::cart::get_dependent_items
( bear::universe::physical_item::item_list& d ) const
{
  super::get_dependent_items(d);
  
  plungers_set::iterator it;
  
  for ( it = m_plungers.begin(); it != m_plungers.end() ; ++it)
    d.push_back( *it );

  d.push_back( m_cursor );
} // cart::get_dependent_items()

/*---------------------------------------------------------------------------*/
/**
 * \brief Add a balloon.
 * \param attracted_item The attracted balloon.
 */
void rp::cart::add_balloon(attractable_item* attracted_item)
{
  if ( m_balloons.size() == s_decorative_balloons_number )
    {
      create_disappear_effect(m_balloons.back());
      m_balloons.pop_back();
    }

  game_variables::set_balloon_red_intensity
    (attracted_item->get_rendering_attributes().get_red_intensity());
  game_variables::set_balloon_green_intensity
    (attracted_item->get_rendering_attributes().get_green_intensity());
  game_variables::set_balloon_blue_intensity
    (attracted_item->get_rendering_attributes().get_blue_intensity());
  game_variables::set_balloons_number
    ( game_variables::get_balloons_number() + 1);

  if ( game_variables::get_balloons_number() ==
       game_variables::get_required_balloons_number() )
    get_level_globals().play_sound( "sound/effect/tiny-bell.ogg");

  std::ostringstream stream;
  stream << "balloon_anchor_" 
         << game_variables::get_balloons_number() % 
    s_decorative_balloons_number + 1;

  decorative_balloon * item = 
    create_decorative_balloon(attracted_item, stream.str());
  m_balloons.push_front(item);

  int depth(2); 
  std::list<decorative_balloon*>::iterator it;
  for ( it = m_balloons.begin(); it != m_balloons.end(); ++it, depth++ )
    (*it)->set_gap_z(-depth);  

  create_link_on_balloon
    (item,stream.str(),game_variables::get_balloons_number());
} // cart::add_balloon()

/*---------------------------------------------------------------------------*/
/**
 * \brief Check if the cart is dead.
 * \return Return true if the cart die.
 */
bool rp::cart::check_death()
{
  bool intact = false;
  std::map<std::string, bool>::iterator it;

  for ( it = m_elements.begin(); ! intact && (it != m_elements.end()); ++it )
    intact = it->second; 
     
  if ( ! intact )
    die(false,false);

  return ! intact;
} // cart::check_death()

/*---------------------------------------------------------------------------*/
/**
 * \brief Lose aelements.
 */
void rp::cart::lose_elements()
{ 
  break_element("back",bear::universe::force_type(300000, 900000));
  break_element("middle",bear::universe::force_type(700000, 1200000));
  break_element("front",bear::universe::force_type(1000000, 900000));

  get_level_globals().play_sound
    ( "sound/hit.ogg", bear::audio::sound_effect(get_center_of_mass()) );
} // cart::lose_elements()

/*---------------------------------------------------------------------------*/
/**
 * \brief Lose balloons.
 * \param nb The number of balloons to lose.
 */
void rp::cart::lose_balloons(unsigned int nb)
{ 
  if ( game_variables::get_balloons_number() > 0 )
    {
      if ( ! m_balloons.empty() )
        {
          balloon* item = new balloon;
          item->set_center_of_mass
            (m_balloons.front()->get_center_of_mass());
          new_item(*item);
          item->get_rendering_attributes() = 
            m_balloons.front()->get_rendering_attributes();
          item->fly(this);

          m_balloons.front()->kill();
          m_balloons.pop_front();
        }
      
      game_variables::set_balloons_number
        ( game_variables::get_balloons_number() - 1);
      
      if ( nb > 1 )
        lose_balloons(nb - 1);
    }
} // lose_balloons()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add sprite representing link between cart and plunger.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::cart::add_plunger_link
( std::list<bear::engine::scene_visual>& visuals ) const
{
  plungers_set::iterator it;

  for ( it = m_plungers.begin(); it != m_plungers.end(); ++it )
    {
       bear::engine::model_mark_placement mark_gun;
       
       if ( get_mark_placement("gun", mark_gun) )
         {
           std::vector<bear::universe::position_type>v;
           v.push_back(mark_gun.get_position());
           v.push_back((*it)->get_tail_position());
           
           bear::visual::scene_line scene =
             bear::visual::scene_line( 0,0,claw::graphic::white_pixel,v, 2 );
           
           bear::engine::scene_visual sc(scene);
           sc.z_position += mark_gun.get_depth_position() + 2;

           visuals.push_front( sc );
         }
    }
} // cart::add_plunger_link()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add sprite representing zone in which plunger can't go.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::cart::add_bad_plunger_zone
( std::list<bear::engine::scene_visual>& visuals ) const
{
  if ( ! m_good_arm_angle && m_bad_plunger_zone_rendering )
    {
      bear::engine::model_mark_placement mark;
      bear::engine::model_mark_placement arm_mark;
      
      if ( ( get_current_action_name() != "with_tar" ) && 
           ( get_current_action_name() != "takeoff" ) && 
           ( get_current_action_name() != "dead" ) &&
           ( get_current_action_name() != "crouch" ) &&
           ! game_variables::is_level_ending() )
        {
          std::vector<bear::visual::position_type> p(4);
          
          p[0] = get_mark_world_position("arm");
          p[1].x = 
            p[0].x + 2000 * cos(0.05 + s_max_arm_angle + get_system_angle());
          p[1].y = 
            p[0].y + 2000 * sin(0.05 + s_max_arm_angle + get_system_angle());
          p[2].x = 
            p[0].x + 2000 * cos(-0.05 + s_min_arm_angle + get_system_angle());
          p[2].y = 
            p[0].y + 2000 * sin(-0.05 + s_min_arm_angle + get_system_angle());
          p[3] = p[0];
          
          bear::visual::scene_polygon e(0, 0, claw::graphic::black_pixel, p);
          e.get_rendering_attributes().set_opacity(0.3);
          visuals.push_back( bear::engine::scene_visual(e) );
        }
    }
} // cart::add_bad_plunger_zone

/*---------------------------------------------------------------------------*/
/**
 * \brief Remove all elements.
 */
void rp::cart::remove_elements()
{
  set_global_substitute("theme back", new bear::visual::animation());
  set_global_substitute("theme middle", new bear::visual::animation());
  set_global_substitute("theme front", new bear::visual::animation());
} // cart::remove_elements()

/*---------------------------------------------------------------------------*/
/**
 * \brief Break an element.
 * \param name The name of broken element.
 * \param The force given at the element.
 */
void rp::cart::break_element
(const std::string& name, const bear::universe::force_type& force)
{
  m_elements[name] = false;

  bear::engine::model_mark_placement element;
  
  if ( get_mark_placement("theme " + name, element) )
    {
      set_global_substitute("theme " + name, new bear::visual::animation());
      
      bear::decorative_item* item = new bear::decorative_item;
      item->set_animation
        ( get_level_globals().auto_sprite( get_theme_image_name(), name ) );
      item->set_size(item->get_sprite().get_size());
      item->set_z_position(element.get_depth_position());
      item->set_system_angle(get_system_angle());
      item->set_system_angle_as_visual_angle(true);
      item->set_mass(10);
      item->set_angular_speed( -4 );
      item->add_external_force(force);
      item->set_density(0.005);
      item->set_artificial(true);
      item->set_phantom(true);
      item->set_kill_when_leaving(true);
      item->set_center_of_mass(element.get_position());
      
      new_item( *item );
      
      CLAW_ASSERT( item->is_valid(),
                   "The decoration of cart isn't correctly initialized" );

      create_hit_star( *item );
    }
} // cart::break_element()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the coordinates of the point to target to (for camera or active
 *        region).
 */
bear::universe::position_type rp::cart::hot_spot() const
{
  bear::universe::position_type p;

  bear::universe::size_type w, h;
  get_action("idle")->get_max_size(w, h);
  const bear::universe::size_box_type half_cart( w / 2, h / 2 );

  if ( get_rendering_attributes().is_mirrored() )
    {
      p = get_bottom_right();
      p.x -= half_cart.x;
    }
  else
    {
      p = get_bottom_left();
      p.x += half_cart.x;
    }

  p.y += half_cart.y;

  return p + m_hot_spot_position;
} // cart::hot_spot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the best position of the hot spot.
 * \param x The position on the X-axis.
 * \param y The position on the Y-axis.
 */
void rp::cart::set_spot_target_position
( bear::universe::coordinate_type x, bear::universe::coordinate_type y )
{
  m_hot_spot_target.set( x, y );
} // cart::set_spot_target_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the maximum movement allowed when balancing the hot spot.
 * \param x Maximum movement on the X-axis.
 * \param y Maximum movement on the Y-axis.
 */
void rp::cart::set_spot_balance_move
( bear::universe::coordinate_type x, bear::universe::coordinate_type y )
{
  m_hot_spot_balance_move.set(std::abs(x), std::abs(y));
} // cart::set_spot_balance_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Change the gap of the spot.
 * \param gap The vector of gap.
 */
void rp::cart::add_spot_gap( const bear::universe::position_type& gap )
{
  if ( m_hot_spot_position.x > m_hot_spot_target.x )
    m_hot_spot_position.x =
      std::max( m_hot_spot_target.x, m_hot_spot_position.x - std::abs(gap.x) );
  else if ( m_hot_spot_position.x < m_hot_spot_target.x )
    m_hot_spot_position.x =
      std::min( m_hot_spot_target.x, m_hot_spot_position.x + std::abs(gap.x) );

  if ( m_hot_spot_position.y > m_hot_spot_target.y )
    m_hot_spot_position.y =
      std::max( m_hot_spot_target.y, m_hot_spot_position.y - std::abs(gap.y) );
  else if ( m_hot_spot_position.y < m_hot_spot_target.y )
    m_hot_spot_position.y =
      std::min( m_hot_spot_target.y, m_hot_spot_position.y + std::abs(gap.y) );
} // cart::add_spot_gap()

/*----------------------------------------------------------------------------*/
/**
 * \brief Balance the gap of the spot.
 * \param x Tell if we balance on the X-axis.
 * \param y Tell if we balance on the Y-axis.
 */
void rp::cart::balance_spot(bool x, bool y)
{
  bear::universe::position_type gap( 0, 0 );

  if ( x )
    gap.x = m_hot_spot_balance_move.x;

  if ( y )
    gap.y = m_hot_spot_balance_move.y;

  add_spot_gap( gap );
} // cart::balance_spot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress the spot.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::cart::progress_spot( bear::universe::time_type elapsed_time )
{
  bool balance_x(true);
  bool balance_y(true);

  if ( get_current_action_name() == "dead" ) 
    {
      balance_y = false;
      
      set_spot_target_position(200, 300);
      add_spot_gap( bear::universe::position_type(15, 3) );
    }
  else if ( !m_on_switching )
    {
      balance_x = true;
      balance_y = true;
      
      const double range = boost::math::constants::pi<double>() / 3;
      const double a =
        ( get_system_angle() < 0 ? -1.0 : 1.0 )
        * std::min( range, std::abs(get_system_angle() ) );
      const double x_bound(500);
      const double y_bound(200);

      if ( a == 0 )
        set_spot_target_position( x_bound, y_bound );
      else if ( a <= 0 ) // moving down
        {
          const double f( a / range );
          set_spot_target_position( x_bound, -y_bound * std::abs(f * f * f) );
        }
      else // moving up
        set_spot_target_position( x_bound * (1 - a / range), y_bound );
    }

  balance_spot(balance_x, balance_y);

  if ( m_spot_item != NULL )
    m_spot_item->set_center_of_mass
      ( get_center_of_mass() + m_hot_spot_position );

  m_on_switching = false;
} // cart::progress_spot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create an item that represents the plunger sight.
 */
void rp::cart::create_cursor()
{
  if ( m_cursor != NULL )
    return;

  m_cursor = new cursor;

  CLAW_ASSERT
    (m_cursor->is_valid(),
     "The base_item created by rp::cart isn't correctly initialized" );

  m_cursor->set_phantom(true);
  m_cursor->set_artificial(false);
  m_cursor->set_global(true);
  m_cursor->set_z_position(100000);

  new_item( *m_cursor );

  m_cursor->set_center_of_mass(get_center_of_mass());
} // cart::create_cursor()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update angle of fire mark item.
 * \param value The angle to give at mark item.
 */
void rp::cart::on_fire_angle_change
(const bear::universe::coordinate_type& value)
{
  set_mark_angle_in_action("cannon",value);
} // cart::create_fire_angle_change()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a decoration of balloon.
 * \param attracted_item The attracted balloon.
 * \param The name of the anchor.
 */
rp::decorative_balloon* rp::cart::create_decorative_balloon
(attractable_item* attracted_item, const std::string& anchor_name)
{
  decorative_balloon* item = new decorative_balloon(this,anchor_name);
  bear::visual::animation anim
    ( get_level_globals().get_animation("animation/balloon-slow.canim") );
  item->set_animation(anim);
  
  if ( attracted_item != NULL )
    item->get_rendering_attributes().set_intensity
      ( attracted_item->get_rendering_attributes().get_red_intensity(),
        attracted_item->get_rendering_attributes().get_green_intensity(),
        attracted_item->get_rendering_attributes().get_blue_intensity());
  else
    item->choose_balloon_color();
  
  item->set_z_position(get_z_position()-2);
  
  new_item( *item );

  bear::engine::model_mark_placement m;

  if ( get_mark_placement(anchor_name, m) )
    item->set_center_of_mass(m.get_position());
  else
    item->set_center_of_mass(get_center_of_mass());

  CLAW_ASSERT( item->is_valid(),
               "The balloon decoration of cart isn't correctly initialized" );

  return item;
} // cart::create_decorative_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the disappear effects on a balloon.
 * \param balloon The balloon.
 */
void rp::cart::create_disappear_effect(decorative_balloon* balloon)
{
  balloon->set_gap_z(-10);

  bear::delayed_kill_item* killer = new bear::delayed_kill_item;
  killer->set_duration(1);
  killer->add_item(balloon);
  new_item(*killer);

  bear::decorative_effect* deco = new bear::decorative_effect;
  deco->set_item(balloon,true);
  deco->set_duration(1);

  bear::visual::color init_color, end_color;
  init_color.set(1,1,1,1);
  end_color.set(1,1,1,0);
  deco->set_color( init_color, end_color );

  new_item(*deco);
} // cart::create_disappear_effect()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a link with a decoration of balloon.
 * \param balloon The balloon.
 * \param anchor_name The name of anchor.
 * \param index The index of the balloon.
 */
void rp::cart::create_link_on_balloon
( decorative_balloon* balloon, const std::string& anchor_name,
  unsigned int index)
{
  new bear::universe::chain_link
    ( bear::engine::model_mark_reference_point<cart>(*this, anchor_name), 
      bear::universe::center_of_mass_reference_point(*balloon),
      0, 60 + 20 * ( index % s_decorative_balloons_number + 1) );
} // cart::create_link_on_balloon()

/*----------------------------------------------------------------------------*/
/**
 * \brief Check if we can throw a cannonball.
 */
bool rp::cart::can_throw_cannonball()
{
  bool result = false;
  bear::engine::model_mark_placement m;

  if ( m_fire_duration >= s_fire_duration )
    if ( get_current_local_mark_placement("cannon", m) )
      {    
        if ( ( get_current_action_name() != "dead" ) && 
             ( get_current_action_name() != "crouch" ) &&
             ( get_current_action_name() != "with_tar" ) &&
             ( get_current_action_name() != "takeoff" ) &&
             game_variables::level_has_started() )
          result = true;
      }
    
  return result;
} // cart::check_throw_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Check if we can throw plunger.
 */
bool rp::cart::can_throw_plunger()
{
  bool result = false;  
  
  bear::engine::model_mark_placement m;

  if ( get_mark_placement("plunger", m) )
    {  
      if ( ( m_plungers.size() < game_variables::get_plunger_total_number()) &&
           ( get_current_action_name() != "dead" ) && 
           ( get_current_action_name() != "crouch" ) &&
           ( get_current_action_name() != "with_tar" ) &&
           ( get_current_action_name() != "takeoff" )  &&
           game_variables::level_has_started() )
        result = true;
    }
  
  return result;
} // cart::check_throw_plunger()

/*----------------------------------------------------------------------------*/
/**
 * \brief Throws a cannonball toward the cursor.
 */
void rp::cart::throw_cannonball()
{
  bear::engine::model_mark_placement mark_fire;

  if ( get_current_local_mark_placement("fire", mark_fire) )
    {
      m_fire_duration = 0;
      
      bear::universe::position_type pos = 
        get_mark_world_position("fire");

      double angle = mark_fire.get_angle() + get_system_angle();

      cannonball* item = new cannonball;
      item->set_z_position(get_z_position()-1);

      bear::universe::vector_type v;
      v.x = cos(angle);
      v.y = sin(angle);
      
      CLAW_ASSERT
        (item->is_valid(),
         "The cannonball created by rp::cart isn't correctly initialized" );

      item->set_center_of_mass(pos);
      new_item( *item );
      item->set_cart(this);
      item->create_movement
        ( v, get_speed(), m_cursor->get_center_of_mass(), m_good_fire );
      
      bear::audio::sound_effect e(get_center_of_mass());
      get_level_globals().play_sound("sound/cart/cannon.ogg", e);

      create_cannon_fire();
      create_wave(false);
    }
} // cart::throw_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Throw a plunger.
 */
void rp::cart::throw_plunger()
{ 
  bear::engine::model_mark_placement mark_arm;
  bear::engine::model_mark_placement mark_plunger;
  
  if ( get_mark_placement("arm", mark_arm) )
    if ( get_mark_placement("plunger", mark_plunger) )
      {
        plunger * p;
        p = new plunger;
        p->set_cart(this);
        p->set_center_of_mass(get_mark_world_position("plunger"));
        bear::universe::vector_type v;
        v.x = cos(m_arm_angle+get_system_angle());
        v.y = sin(m_arm_angle+get_system_angle());
        p->set_system_angle(m_arm_angle+get_system_angle());
        p->create_movement(v, get_speed());
        p->set_z_position(get_z_position()-1);
        
        CLAW_ASSERT
          (p->is_valid(),
           "The plunger created by rp::cart isn't correctly initialized" );
        
        new_item( *p );
        m_plungers.insert(p);
        
        bear::audio::sound_effect e(get_center_of_mass());
        get_level_globals().play_sound("sound/shot.ogg", e);
        create_wave(true);
      }
} // cart::throw_plunger()

/*----------------------------------------------------------------------------*/
/**
 * \brief Init all element states : intact or non intact.
 */
void rp::cart::init_elements()
{
  init_element("middle");
  init_element("back");
  init_element("front");
} // cart::init_elements()

/*----------------------------------------------------------------------------*/
/**
 * \brief Init an element state : intact or not.
 * \param name The name of the element.
 */
void rp::cart::init_element(const std::string& name)
{
  m_elements[name] = true;

  bear::engine::model_mark_placement element;
  
  if ( get_mark_placement( "theme " + name, element ) )
    set_global_substitute
      ( "theme " + name,
        new bear::visual::animation
        ( get_level_globals().auto_sprite( get_theme_image_name(), name ) ) );
} // cart::init_element()

/*----------------------------------------------------------------------------*/
/**
 * \brief Kill all decorative balloons.
 */
void rp::cart::clear_balloons()
{ 
  std::list<decorative_balloon*>::const_iterator it;

  for ( it = m_balloons.begin(); it != m_balloons.end(); ++it)
    (*it)->kill();

  m_balloons.clear();
} // cart::clear_balloons()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give force movement.
 */
void rp::cart::give_force_movement()
{ 
  // give internal force
  bear::universe::force_type force
    ( m_ground_force * (1 + 2 * std::sin( get_system_angle() ) ) );

  if ( get_current_action_name() == "crouch" )
    force *= 2;
 
  if ( get_speed().x < 0 )
    add_internal_force(8*force*m_force_factor);
  else
    add_internal_force(force*m_force_factor);
  
  // check minimal speed 
  bear::universe::coordinate_type min_length( s_min_speed_length );
  
  if ( get_current_action_name() == "crouch" )
    min_length *= 2;

  if ( ( get_speed().length() < min_length ) && 
       ( get_system_angle() > 0.5 ) )
    {
      bear::universe::speed_type dir
        ( cos(get_system_angle()), sin(get_system_angle()) );
      dir *= min_length;
      
      set_speed(dir);
    }
} // cart::give_force_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give force movement in sky.
 */
void rp::cart::give_sky_force_movement()
{
  bear::universe::force_type force(50000,0);
  if ( ! m_right_orientation )
    force.x *= -1;
  
  add_internal_force(force);
} // cart::give_force_movement()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give force movement in looping.
 */
void rp::cart::give_looping_force_movement()
{
  bear::universe::speed_type dir;

  if ( has_right_contact() ) 
    dir = bear::universe::speed_type(0,1);
  else if ( has_top_contact() && (get_system_angle() == 0) ) 
    {
      set_system_angle(3.14);
      if ( get_speed().x > 0 ) 
	dir = bear::universe::speed_type(-1,0);
      else
	dir = bear::universe::speed_type(1,0);
    }
  else
    dir = bear::universe::speed_type
      ( cos(get_system_angle()), sin(get_system_angle()) );
  
  bear::universe::coordinate_type length = get_speed().length();

  if ( get_speed().y > 0 )
    length -= 20;
  else
    length += 50;
  
  if ( length > 1500 )
    length = 1500;
  else if ( length < 800 )
    length = 800;
  set_speed(dir * length);
} // cart::give_looping_movement()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration on tweeners in the progression of the item.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::cart::progress_tweeners( bear::universe::time_type elapsed_time )
{
  m_tweener_fire_angle.update(elapsed_time);
} // cart::progress_tweener()

/*---------------------------------------------------------------------------*/
/**
 * \brief Do one iteration for injured state updating.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::cart::progress_injured_state( bear::universe::time_type elapsed_time )
{
  if ( m_is_injured )
    {
      m_injured_duration += elapsed_time;

      if ( m_injured_duration > s_injured_duration )
        init_injured_state();
    }
} // cart::progress_injured_state()

/*---------------------------------------------------------------------------*/
/**
 * \brief Progress arm angle.
 */
void rp::cart::progress_arm_angle()
{
  bear::engine::model_mark_placement m;

  if ( ( m_cursor != NULL ) &&
       get_mark_placement("arm", m) && 
       ( get_current_action_name() != "dead" ) )
    {
      bear::universe::position_type sight = m_cursor->get_target();
      bear::universe::position_type pos =
        get_mark_world_position("arm");
      double angle = 
        std::atan2( sight.y - pos.y, sight.x - pos.x) - get_system_angle();
      if ( angle <= - 3.1416 )
        angle += 6.2832;
      
      m_good_arm_angle = false;
      if ( angle > s_max_arm_angle + 0.05 ) 
        angle = s_max_arm_angle;
      else if ( angle < s_min_arm_angle - 0.05 )
	angle = s_min_arm_angle;
      else
        m_good_arm_angle = true;

      set_mark_angle_in_action("arm",angle);
      m_arm_angle = angle;
    }
} // cart::progress_arm_angle()

/*---------------------------------------------------------------------------*/
/**
 * \brief Progress plunger.
 */
void rp::cart::progress_plunger()
{
  bear::engine::model_mark_placement arm_mark;
  bear::engine::model_mark_placement plunger_mark;

  if ( get_mark_placement("plunger", plunger_mark) && 
       get_mark_placement("arm", arm_mark) )
    {
      set_mark_visibility_in_action
        ("plunger", 
         (m_plungers.size() < game_variables::get_plunger_total_number() ) && 
         (get_current_action_name() != "crouch" ) );
      set_mark_position_in_action("plunger", get_plunger_position());
      set_mark_angle_in_action("plunger", m_arm_angle);
    }
} // cart::progress_plunger()

/*---------------------------------------------------------------------------*/
/**
 * \brief Progress cannon.
 */
void rp::cart::progress_cannon()
{  
  bear::engine::model_mark_placement m;

  if ( ( m_cursor != NULL ) &&
       get_mark_placement("cannon", m) && 
       get_current_action_name() != "dead" )
    {
      bear::universe::position_type pos =
        get_mark_world_position("cannon");

      double angle = 
	std::atan2
	( m_cursor->get_vertical_middle() - pos.y,
	  m_cursor->get_horizontal_middle() - pos.x);
      
      m_good_fire = false;
      if ( ( angle > s_max_cannon_angle + get_system_angle() ) || 
           ( angle < - 3.14 + get_system_angle() ) )
        angle = s_max_cannon_angle;
      else if ( angle < s_min_cannon_angle + get_system_angle() )
        angle = s_min_cannon_angle;
      else 
        {
          angle -= get_system_angle();
          m_good_fire = ( m_fire_duration >= s_fire_duration );
        }

#ifdef __ANDROID__
      on_fire_angle_change( angle );
#else
      m_tweener_fire_angle = claw::tween::single_tweener
	( m.get_angle() - get_system_angle(), angle, 0.2,
          boost::bind( &rp::cart::on_fire_angle_change, this, _1 ), 
          &claw::tween::easing_cubic::ease_out );
#endif
    }
} // cart::progress_cannon()

/*---------------------------------------------------------------------------*/
/**
 * \brief Progress fire.
 */
void rp::cart::progress_fire()
{
  bear::engine::model_mark_placement cannon_mark;
  bear::engine::model_mark_placement fire_mark;

  if ( get_current_local_mark_placement("fire", fire_mark) && 
       get_current_local_mark_placement("cannon", cannon_mark) )
    {
      set_mark_position_in_action("fire", compute_fire_position());
      set_mark_angle_in_action("fire", cannon_mark.get_angle());
    }
} // cart::progress_fire()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state idle.
 */
void rp::cart::progress_idle( bear::universe::time_type elapsed_time )
{
  // do nothing
} // cart::progress_idle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state move.
 */
void rp::cart::progress_move( bear::universe::time_type elapsed_time )
{
  if ( has_bottom_contact() ) 
    {
      if ( ! m_looping )
	give_force_movement();
      else
	give_looping_force_movement();

      check_crouch();
    }
  else 
    test_in_sky();
} // cart::progress_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state jump.
 */
void rp::cart::progress_jump( bear::universe::time_type elapsed_time )
{
  if ( has_bottom_contact() )
     check_crouch();
  else if( get_speed().y <= 0 )
    start_model_action("fall");

  if ( m_looping )
    give_looping_force_movement();
  else
    give_sky_force_movement();
} // cart::progress_jump()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state fall.
 */
void rp::cart::progress_fall( bear::universe::time_type elapsed_time )
{
  if ( has_bottom_contact() )
    check_crouch();
  else if( get_speed().y > 0 )
    start_model_action("jump");

  if ( m_looping )
    give_looping_force_movement();
  else
    give_sky_force_movement();
} // cart::progress_fall()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state crouch.
 */
void rp::cart::progress_crouch( bear::universe::time_type elapsed_time )
{
  if ( has_bottom_contact() ) 
    give_force_movement();
  else 
    test_in_sky();
} // cart::progress_crouch()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state dead.
 */
void rp::cart::progress_dead( bear::universe::time_type elapsed_time )
{
  if ( m_right_orientation ) 
    set_system_angle(get_system_angle() - 0.1);
  else
    set_system_angle(get_system_angle() + 0.1);
} // cart::progress_dead()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state takeoff.
 */
void rp::cart::progress_takeoff( bear::universe::time_type elapsed_time )
{
  if( has_bottom_contact() )
    {
      if ( get_system_angle() == 0 && game_variables::is_boss_level() )
        {
          set_artificial(true);
          set_density(0.00095);
        }
      give_force_movement();
    }
  else
    {
      add_internal_force( m_ground_force / 6 );

      if ( game_variables::get_balloons_number() >=
           game_variables::get_required_balloons_number() )
        {
          if ( (m_takeoff_duration >= 1)
               && ( (int)(m_takeoff_duration + elapsed_time)
                    != (int)m_takeoff_duration) )
            create_rocket();
        }
      else if ( m_takeoff_duration > 3.5 )
        {
          set_density( 0.005 );
          lose_balloons( 5 );
          die( false, false );
        }

      m_takeoff_duration += elapsed_time;
    }
} // cart::progress_takeoff()

/*----------------------------------------------------------------------------*/
/**
 * \brief Progress in the state dead.
 */
void rp::cart::progress_with_tar( bear::universe::time_type elapsed_time )
{
  if ( has_bottom_contact() ) 
    {
      if ( ! m_looping )
	give_force_movement();
      else
	give_looping_force_movement();
    }
} // cart::progress_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Check if the cart must be in crouch state.
 */
void rp::cart::check_crouch()
{
  if ( m_want_crouch ) 
    start_model_action("crouch");
  else if ( get_current_action_name() != "move" )
    start_model_action("move");
} // cart::check_crouch()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start idle state.
 */
void rp::cart::start_idle()
{
  m_progress = &rp::cart::progress_idle;
} // cart::start_idle()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start move state.
 */
void rp::cart::start_move()
{
  m_progress = &rp::cart::progress_move;
} // cart::start_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start jump state.
 */
void rp::cart::start_jump()
{
  m_progress = &rp::cart::progress_jump;
} // cart::start_jump()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start fall state.
 */
void rp::cart::start_fall()
{
  m_progress = &rp::cart::progress_fall;
} // cart::start_fall()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start crouch state.
 */
void rp::cart::start_crouch()
{
  m_progress = &rp::cart::progress_crouch;
} // cart::start_crouch()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start dead state.
 */
void rp::cart::start_dead()
{
  m_progress = &rp::cart::progress_dead;
} // cart::start_dead()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start takeoff state.
 */
void rp::cart::start_takeoff()
{
  std::list<decorative_balloon*>::iterator it;
  for ( it = m_balloons.begin(); it != m_balloons.end(); ++it )
    (*it)->fly(0.5,0,false);  

  unsigned int nb_balloons(game_variables::get_balloons_number());

  for ( unsigned int i = 1; i <= nb_balloons; ++i )
    {
      std::ostringstream stream;
      stream << "ending_balloon_anchor_" << i % 10 + 1;

      decorative_balloon * item = create_decorative_balloon(NULL,stream.str());
      
      m_balloons.push_front(item);
      
      item->fly
        ( (double)rand() * 3 / RAND_MAX, 
          50 + (double)rand() * 200 / RAND_MAX,true);
    }

  m_takeoff_duration = 0;
} // cart::start_takeoff()

/*----------------------------------------------------------------------------*/
/**
 * \brief Start with_tar state.
 */
void rp::cart::start_with_tar()
{
  m_progress = &rp::cart::progress_with_tar;
} // cart::start_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply impulsion of a jump.
 */
void rp::cart::apply_impulse_jump()
{
  start_model_action("jump");
  give_impulse_force();
  
  const bear::audio::sound_effect e(get_center_of_mass());
  get_level_globals().play_sound("sound/cart/andy/jump.ogg", e);
} // player::apply_impulse_jump()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give impulsion of a jump.
 */
void rp::cart::give_impulse_force()
{
  bear::universe::force_type force;
  force.x = 2000000 + 7000000*sin(get_system_angle()); 

  if ( get_system_angle() < 0.8 )
    force.y = 7000000 + 1000000*sin(get_system_angle());
  else
    force.y = 2000000 + 1000000*sin(get_system_angle());

  if ( ! m_right_orientation )
    force.x *= -1;
  
  add_internal_force(force);
} // cart::give_impulse_force()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply crouch action.
 */
void rp::cart::apply_crouch()
{
  m_want_crouch = true;

  if ( ( ( get_current_action_name() == "idle" ) ||
         ( get_current_action_name() == "move" ) ) && 
       m_plungers.empty() )
    start_model_action("crouch");
} // player::apply_crouch()

/*----------------------------------------------------------------------------*/
/**
 * \brief Apply stop crouch action.
 */
void rp::cart::apply_stop_crouch()
{
  m_want_crouch = false;

  if ( get_current_action_name() == "crouch" )
    start_model_action("move");
} // player::apply_crouch()

/*----------------------------------------------------------------------------*/
/**
 * \brief Method called when a key is pressed.
 * \param key The value of the key.
 * \return true if the key has been processed.
 *
 * This methods use raw key codes as \a key. If you want the character as
 * entered by the user, use char_pressed.
 */
bool rp::cart::key_pressed( const bear::input::key_info& key )
{
  bool result = true;

  if ( ( key.is_shift() || key.is_up() ) && 
       game_variables::level_has_started() )
    {
      if ( game_variables::level_has_started() &&
           ! game_variables::is_level_ending() &&
           m_can_jump && 
           ( ( get_current_action_name() == "move" ) ||
             ( get_current_action_name() == "crouch" ) ) )
        apply_impulse_jump();
    }
  else if ( game_variables::level_has_started() && 
            ( key.is_down() || key.is_control() )  )
    apply_crouch();
  else
    result = super::key_pressed(key);
  
  return result;
} // cart::key_pressed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the player to stop the action associated with a keyboard key.
 * \param key The code of the key.
 */
bool rp::cart::key_released( const bear::input::key_info& key )
{
  bool result = true;

  if ( ( key.is_control() || key.is_down() ) && 
       game_variables::level_has_started() )
    apply_stop_crouch();
  else 
#ifndef NDEBUG
    if ( key.is_end() )
      {
        set_center_of_mass
          ( bear::universe::position_type
            (get_level().get_size().x - 2300, 
             get_level().get_size().y ) );
        
        game_variables::set_balloons_number
          (game_variables::get_required_balloons_number());
      }
    else
#endif
      result = super::key_released(key);

  return result;
} // cart::key_released()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell the player to stop the action associated with a mouse button.
 * \param button The code of the button.
 * \param pos The position of the cursor on the screen.
 */
bool rp::cart::mouse_released
( bear::input::mouse::mouse_code button,
  const claw::math::coordinate_2d<unsigned int>& pos )
{
#ifdef __ANDROID__
  // The android build handles the input in finger_action().
  return false;
#endif

  if ( !game_variables::level_has_started() )
    return false;

  bool result = true;

  switch( button )
    {
    case  bear::input::mouse::mc_right_button:
      input_handle_cannonball();
      break;
    case  bear::input::mouse::mc_left_button:
      input_handle_plunger();
      break;
    case  bear::input::mouse::mc_wheel_up:
      input_handle_jump();
      break;
    case  bear::input::mouse::mc_wheel_down:
      input_handle_crouch();
      break;
    default:
      result = false;
    }

  return result;
} // cart::mouse_released()

/*----------------------------------------------------------------------------*/
/**
 * \brief Method called when the position of the mouse changes.
 * \param pos The new position of the cursor.
 * \return true if the event has been processed.
 */
bool rp::cart::mouse_move
( const claw::math::coordinate_2d<unsigned int>& pos )
{
#ifdef __ANDROID__
  return false;
#endif

  update_cursor_position( pos );

  return true;
} // cart::mouse_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Processes an input coming from a finger.
 * \param event The event to process.
 */
bool rp::cart::finger_action( const bear::input::finger_event& event )
{
  if ( !game_variables::level_has_started() )
    return false;

  if ( event.get_type() == bear::input::finger_event::finger_event_pressed )
    {
      m_cursor_down = true;
      m_cursor_down_screen_position = event.get_position();
      update_cursor_position( m_cursor_down_screen_position );
      return true;
    }

  if ( event.get_type() != bear::input::finger_event::finger_event_released )
    return false;

  m_cursor_down = false;

  const bear::universe::position_type delta
    ( event.get_position() - m_cursor_down_screen_position );
  const bear::universe::size_type length
    ( delta.distance( bear::universe::position_type(0, 0) ) );

  if ( length < 80 )
    {
      input_handle_plunger();
      return true;
    }

  const double jump_min_cos_angle
    ( std::cos( boost::math::constants::pi<double>() / 3 ) );

  if ( std::abs( delta.x / length ) < jump_min_cos_angle )
    {
      if ( delta.y < 0 )
        input_handle_crouch();
      else
        input_handle_jump();
    }
  else
    input_handle_cannonball();

  return true;
} // cart::finger_action()

/*----------------------------------------------------------------------------*/
/**
 * \brief Answers to an input by throwing a cannonball.
 */
void rp::cart::input_handle_cannonball()
{
  if ( get_current_action_name() == "crouch" )
    apply_stop_crouch();
  else if ( can_throw_cannonball() )
    throw_cannonball();
  else
    {
      const bear::audio::sound_effect e(get_center_of_mass());;
      get_level_globals().play_sound("sound/cart/empty-cannon.ogg", e);
    }
} // cart::input_handle_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Answers to an input by throwing a plunger.
 */
void rp::cart::input_handle_plunger()
{
  if ( get_current_action_name() == "crouch" )
    apply_stop_crouch();
  else if ( can_throw_plunger() )
    throw_plunger();
  else 
    {
      const bear::audio::sound_effect e(get_center_of_mass());;
      get_level_globals().play_sound("sound/empty.ogg", e);
    }
} // cart::input_handle_plunger()

/*----------------------------------------------------------------------------*/
/**
 * \brief Answers to an input by jumping.
 */
void rp::cart::input_handle_jump()
{
  if ( get_current_action_name() == "crouch" )
    apply_stop_crouch();
  else if ( !game_variables::is_level_ending() && m_can_jump && 
            ( ( get_current_action_name() == "move" ) ||
              ( get_current_action_name() == "crouch" ) ) )
    apply_impulse_jump();
} // cart::input_handle_jump()

/*----------------------------------------------------------------------------*/
/**
 * \brief Answers to an input by crouching.
 */
void rp::cart::input_handle_crouch()
{
  apply_crouch();
} // cart::input_handle_crouch()

/*----------------------------------------------------------------------------*/
/**
 * \brief Places the cursor at such that it appears at a given position on the
 *        screen.
 * \param screen_position The position on the screen.
 */
void rp::cart::update_cursor_position
( const bear::universe::position_type& screen_position )
{
  m_gap_mouse.x = 
    screen_position.x
    / bear::engine::game::get_instance().get_window_size().x;
  m_gap_mouse.y = 
    screen_position.y
    / bear::engine::game::get_instance().get_window_size().y;

  m_cursor->set_center_of_mass
    ( bear::universe::position_type
      ( get_level().get_camera_focus().left() + 
        m_gap_mouse.x * get_level().get_camera_focus().width(),
        get_level().get_camera_focus().bottom() + 
        m_gap_mouse.y * get_level().get_camera_focus().height() ) );
} // cart::update_cursor_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief The toggle is activated, kill the player.
 * \param activator The item who activated this toggle.
 */
void rp::cart::on_toggle_on
( bear::engine::base_item* activator )
{
  super::on_toggle_on(activator);

  if ( get_current_action_name() == "idle" )
    start_model_action("move");
} // cart::on_toggle_on()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with painter.
 * \param mark The mark on which the collision occurred.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::cart::on_painter_collision
( bear::engine::base_item& mark, bear::engine::base_item& that,
  bear::universe::collision_info& info )
{
  if ( get_current_action_name() != "dead" ) 
    {
      hole* p = dynamic_cast<hole*>(&that);

      if ( p != NULL )
	die( info.get_collision_side() == 
             bear::universe::zone::middle_right_zone,
             info.get_collision_side() == 
             bear::universe::zone::middle_left_zone );
      else if ( ! collision_with_tar(that) )
        {
          bird* b = dynamic_cast<bird*>(&that);

          if ( b != NULL )
            b->collision(*this,info);
          else
             {
               obstacle* o = dynamic_cast<obstacle*>(&that);
               
               if ( o != NULL )
                 o->collision(*this,info);
               else
                 {
                   bomb* bo = dynamic_cast<bomb*>(&that);
                   
                   if ( bo != NULL )
                     bo->collision(*this,info);
                   else
                     {
                       zeppelin* z = dynamic_cast<zeppelin*>(&that);
                       
                       if ( z != NULL )
                         z->collision(*this,info);
                     }
                 }
             }
        }
    }
} // cart::on_painter_collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision.
 * \param that The other item in the collision.
 * \param info Some informations about the collision.
 */
void rp::cart::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  m_speed_on_contact = get_speed();

  switching* s = dynamic_cast<switching*>(&that);
  if ( s != NULL ) 
    m_on_switching = true;
  else if ( ! collision_with_tar(that) )
    if ( !collision_with_explosion(that, info) )
      collision_with_cannonball(that);
} // cart::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create a decorative_item on cursor.
 * \param double_wave Indicates if we create the double_wave.
 */
void rp::cart::create_wave( bool double_wave )
{
  if ( ! game_variables::is_boss_transition() )
    {
      bear::decorative_item* item = new bear::decorative_item;
      if ( double_wave )
        {
          item->set_animation
            ( get_level_globals().get_animation
              ("animation/effect/double-wave.canim") );
          item->get_rendering_attributes().colorize
            ( bear::visual::color( "#ffb000" ) );
        }
      else
        {
          item->set_animation
            ( get_level_globals().get_animation
              ("animation/effect/wave.canim") );
          item->get_rendering_attributes().colorize
            ( bear::visual::color( "#b92c07" ) );
        }
      
      item->set_size(item->get_sprite().get_size());
      item->set_z_position(m_cursor->get_z_position() - 1);
      item->set_artificial(true);
      item->set_phantom(true);
      item->set_kill_when_leaving(true);
      item->set_kill_when_finished(true);
      item->set_center_of_mass(m_cursor->get_center_of_mass());
      
      new_item( *item );
      
      CLAW_ASSERT
        ( item->is_valid(),
          "The decoration od wave of cart isn't correctly initialized" );
    }
} // cart::create_wave()

/*---------------------------------------------------------------------------*/
/**
 * \brief Creates some smoke on the cart, if needed.
 * \param elapsed_time The elapsed time since the last check.
 */
void rp::cart::create_smoke( double elapsed_time )
{
  const std::size_t health = game_variables::get_cart_elements_number(m_id);

  if ( health == 3 )
    m_next_smoke = 0;
  else
    {
      m_next_smoke -= elapsed_time;

      if ( m_next_smoke <= 0 )
        {
          if ( health == 2 )
            {
              util::create_smoke( *this, 1, 0.3, 0.8, -1 );
              m_next_smoke = s_smoke_delay;
            }
          else if ( health == 1 )
            {
              util::create_smoke( *this, 2, 0.1, 0.3, -1 );
              m_next_smoke = s_smoke_delay / 2;
            }
        }
    }
} // cart::create_smoke()

/*---------------------------------------------------------------------------*/
/**
 * \brief Update the value of m_previous_bottom_contact and play a sound if
 *        needed.
 */
void rp::cart::update_bottom_contact()
{
  if ( ( !m_previous_bottom_contact && has_bottom_contact() 
       && (m_speed_on_contact.y < 0) ) || 
       ( !m_previous_top_contact && has_top_contact() 
	 && (m_speed_on_contact.y > 0) ) )
    {
      const double sound_selector = (double)rand() / RAND_MAX;
      const bear::audio::sound_effect e(get_center_of_mass());;

      if ( sound_selector < 1.0 / 3.0 )
        get_level_globals().play_sound("sound/metal-2.ogg", e);
      else if ( sound_selector < 2.0 / 3.0 )
        get_level_globals().play_sound("sound/metal-3.ogg", e);
      else
        get_level_globals().play_sound("sound/metal-4.ogg", e);
    }

  m_previous_bottom_contact = has_bottom_contact();
  m_previous_top_contact = has_top_contact();
} // cart::update_bottom_contact()

/*---------------------------------------------------------------------------*/
/**
 * \brief Update informations required by status layer.
 */
void rp::cart::update_status_informations()
{
  // cannonball
  bool cannonball_activation = can_throw_cannonball();
  if ( cannonball_activation != game_variables::get_cannonball_activation() )
    game_variables::set_cannonball_activation( cannonball_activation );

  bool cannonball_validity = m_good_fire && can_throw_cannonball();
  if ( cannonball_validity != game_variables::get_cannonball_validity() )
        game_variables::set_cannonball_validity( cannonball_validity );

  // plunger
  unsigned int plunger_number = 
    game_variables::get_plunger_total_number() - m_plungers.size();
  if ( plunger_number != game_variables::get_plunger_number() )
    game_variables::set_plunger_number( plunger_number );

  bool plunger_activation = can_throw_plunger();
  if ( plunger_activation != game_variables::get_plunger_activation() )
        game_variables::set_plunger_activation( plunger_activation );
  
  bool plunger_validity = can_throw_plunger() && m_good_arm_angle;
  if ( plunger_validity != game_variables::get_plunger_validity() )
        game_variables::set_plunger_validity( plunger_validity ); 
} // cart::update_status_information()

/*---------------------------------------------------------------------------*/
/**
 * \brief Test if rp::cart is in the sky and change state thereof.
 * Return true if Player is in the sky.
 */
bool rp::cart::test_in_sky()
{
  bool result = false;

  if ( !has_bottom_contact() )
    {
      result = true;

      if( get_speed().y <= 0 )
        start_model_action("fall");
      else
        start_model_action("jump");
    }
  
  return result;
} // cart::test_in_sky()

/*---------------------------------------------------------------------------*/
/**
 * \brief Compute gun position.
 */
bear::universe::position_type rp::cart::compute_gun_position() const
{
  bear::engine::model_mark_placement arm_mark;

  if ( get_current_local_mark_placement("arm", arm_mark) )
    {
      bear::universe::position_type pos = arm_mark.get_position();

      pos.x += 63 * cos(m_arm_angle) - 2 * sin(m_arm_angle);
      pos.y += 63 * sin(m_arm_angle) + 2 * cos(m_arm_angle);
      
      return pos;
    }
  else
    return bear::universe::position_type();
} // cart::compute_gun_position()

/*---------------------------------------------------------------------------*/
/**
 * \brief Compute position of fire.
 */
bear::universe::position_type rp::cart::compute_fire_position() const
{
  bear::engine::model_mark_placement cannon_mark;

  if ( get_current_local_mark_placement("cannon", cannon_mark) )
    {
      bear::universe::position_type pos = cannon_mark.get_position();
      pos.x += 58 * cos(cannon_mark.get_angle()) ;
      pos.y += 58 * sin(cannon_mark.get_angle());
      
      return pos;
    }
  else
    return bear::universe::position_type();
} // cart::compute_fire_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the name of the image to use for the theme of the cart.
 */
std::string rp::cart::get_theme_image_name() const
{
  return "gfx/cart-theme/" + game_variables::get_level_theme() + ".png";
} // cart::get_theme_image_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a tar.
 * \param that The other item of the collision.
 * \return Return True if the collision is with a tar.
 */
bool rp::cart::collision_with_tar( bear::engine::base_item& that )
{ 
  bool result = false;
  tar* t = dynamic_cast<tar*>(&that);
  
  if ( t != NULL ) 
    {
      if ( ( get_current_action_name() != "with_tar" )  && 
           ( ( t->get_current_action_name() == "idle" ) || 
             ( t->get_current_action_name() == "fall" ) ) )
        {
          start_model_action("with_tar");
          get_level_globals().play_sound
            ( "sound/tar/splash.ogg",
              bear::audio::sound_effect( t->get_center_of_mass() ) );

          t->kill();
        }

      result = true;
    }
  
  return result;
} // cart::collision_with_tar()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an explosion.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with an explosion.
 */
bool rp::cart::collision_with_explosion
( bear::engine::base_item& that, bear::universe::collision_info& info  )
{ 
  bool result = false;
  explosion* e = dynamic_cast<explosion*>(&that);
  
  if ( e != NULL ) 
    {
      if ( e->test_in_explosion(info) )
        is_hit();

      result = true;
    }
  
  return result;
} // cart::collision_with_explosion()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cannonball
 * \param that The other item of the collision.
 * \return Return True if the collision is with an explosion.
 */
bool rp::cart::collision_with_cannonball( bear::engine::base_item& that )
{ 
  bool result = false;

  cannonball* c = dynamic_cast<cannonball*>(&that);
  
  if ( c != NULL )
    {
      if ( m_passive && !m_is_injured )
        {
          is_hit();
          give_impulse_force();
          create_balloons( 10 );
        }

      result = true;
    }
  
  return result;
} // cart::collision_with_cannonball()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates some balloons as if they where released from the cart.
 * \param count The number of balloons to create.
 */
void rp::cart::create_balloons( std::size_t count ) const
{ 
  for ( std::size_t i(0); i!=count; ++i )
    {
      balloon* item = new balloon;
      item->set_center_of_mass( get_center_of_mass() );
      item->set_artificial( true );

      const bear::universe::force_type f
        ( ((double)rand() / RAND_MAX + 1) * 5000,
          ((double)rand() / RAND_MAX + 1) * 2000 );

      item->add_internal_force( f );

      new_item(*item);
    }
} // cart::create_balloons()

/*----------------------------------------------------------------------------*/
/**
 * \brief Init injure state parameters.
 */
void  rp::cart::init_injured_state()
{
  m_is_injured = false;
  m_injured_duration = 0;
} // cart::init_injured_state()

/*----------------------------------------------------------------------------*/
/**
 * \brief Check the takeoff.
 */
void rp::cart::check_takeoff()
{
  if ( ! game_variables::is_level_ending() )
    {
      game_variables::set_level_ending(true);
      
      set_density(0.00097);
      
      m_takeoff_duration = 0;
      
      get_level_globals().stop_all_musics(0.5);
      
      if ( game_variables::get_balloons_number() >= 
           game_variables::get_required_balloons_number())
        {
          set_artificial(true);
          set_system_angle(0);
          bear::engine::transition_effect_message<level_ending_effect> msg;
          
          if ( has_world() )
            msg.get_effect().set_world( &get_world() );
          
          msg.get_effect().set_cart(this);
          
          get_level_globals().send_message
            ( RP_TRANSITION_EFFECT_DEFAULT_TARGET_NAME, msg );
          
          get_level_globals().play_music("music/cart/win.ogg", 1 );
        }
      else
        {
          util::send_fail_level(get_level().get_filename());
          get_level_globals().play_music( "music/cart/lose.ogg", 1 );
        }
    }
} // cart::check_takeoff()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates a rocket for the end of the level.
 */
void rp::cart::create_rocket() const
{
  bear::rocket* main_rocket = new bear::rocket();

  main_rocket->set_size( 10, 10 );
#if defined( __ANDROID__ )
  main_rocket->set_explosion_rocket_count( 5, 10 );
#else
  main_rocket->set_explosion_rocket_count( 10, 20 );
#endif
  main_rocket->set_trace( 0.25, get_random_rocket_color(), 1 );
  main_rocket->set_explosion_date( 0.5, 0.75 );
  main_rocket->set_kill_when_leaving( true );

  double max_force_factor;

  if ( game_variables::is_boss_level() )
    max_force_factor = 3.5;
  else
    max_force_factor = 2.5;

  main_rocket->set_force_factor( 1.75, max_force_factor );
  main_rocket->set_angle( -0.3, 0.3 );

  const bear::universe::rectangle_type camera( get_level().get_camera_focus() );

  main_rocket->set_center_of_mass
    ( camera.left() + camera.width() / 4
      + (camera.width() / 2) * (double)rand() / RAND_MAX,
      camera.bottom() );

  main_rocket->set_z_position( get_z_position() * 2 );

  main_rocket->set_explosion_sound_name( get_rocket_explosion_sound_name() );
  main_rocket->add_rocket( *create_small_rocket() );

  new_item( *main_rocket );
} // cart::create_rocket()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates a small rocket, to be passed to a big rocket.
 */
bear::rocket* rp::cart::create_small_rocket() const
{
  bear::rocket* r = new bear::rocket();

  r->set_size( 5, 5 );
  r->set_trace( 0.2, get_random_rocket_color(), 1 );

#if defined( __ANDROID__ )
  r->set_explosion_date( 0.9, 1 );
#else
  r->set_explosion_date( 0.9, 1.5 );
#endif

  return r;
} // cart::create_small_rocket()

/*----------------------------------------------------------------------------*/
/**
 * \brief Finds a color for a rocket.
 */
bear::visual::color rp::cart::get_random_rocket_color() const
{
  const std::size_t color_count(6);

  bear::visual::color colors[ color_count ] =
    {
      bear::visual::color("#f0c000"), // orange
      bear::visual::color("#f0f080"), // yellow
      bear::visual::color("#c0f0c0"), // bright green
      bear::visual::color("#80f0f0"), // bright blue
      bear::visual::color("#8080f0"), // medium blue
      bear::visual::color("#f080f0")  // pink
    };

  return colors[ rand() % color_count ];
} // cart::get_random_rocket_color()

/*----------------------------------------------------------------------------*/
/**
 * \brief Finds a sound for the explosion of a rocket.
 */
std::string rp::cart::get_rocket_explosion_sound_name() const
{
  const std::size_t i( rand() % 4 + 1);

  std::ostringstream oss;
  oss << "sound/fireworks/explosion-" << i << ".ogg";

  return oss.str();
} // cart::get_rocket_explosion_sound_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set all signals to listen.
 */
void rp::cart::init_signals()
{
  m_signals.push_back
    ( bear::engine::game::get_instance().listen_uint_variable_change
      ( game_variables::get_last_combo_variable_name(),
        boost::bind(&rp::cart::on_combo_changed, this, _1) ) );
} // cart::init_signals()

/*----------------------------------------------------------------------------*/
/**
 * \brief The fonction called when the combo changes.
 * \param value The new combo value.
 */
void rp::cart::on_combo_changed(unsigned int combo)
{
  if ( combo >= 3 )
    {
      if ( m_combo_sample != NULL )
        {
          if ( ! m_combo_sample->is_playing() )
            {
              delete m_combo_sample;
              m_combo_sample = NULL;
            }
        }
      
      if ( m_combo_sample == NULL )
        {
          m_combo_sample = 
            get_level_globals().new_sample
            ( get_combo_sample_name( combo ) );
          m_combo_sample->play
            ( bear::audio::sound_effect( get_center_of_mass() ) );
        }      
    }
} // cart::on_combo_changed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the name of the combo sample to play.
 * \param value The new combo value.
 */
std::string rp::cart::get_combo_sample_name( unsigned int combo ) const
{
  std::string result;
  const double sound_selector = (double)rand() / RAND_MAX;
 
  if ( combo == 3 )
    {
      if ( sound_selector < 1.0 / 3.0 )
        result = "sound/cart/andy/cool.ogg";
      else if ( sound_selector < 2.0 / 3.0 )
        result = "sound/cart/andy/whoo.ogg";
      else
        result = "sound/cart/andy/yeah-1.ogg";
    }
  else if ( combo <= 6 )
    { 
      if ( sound_selector < 1.0 / 4.0 )
        result = "sound/cart/andy/yeah-2.ogg";
      else if ( sound_selector < 2.0 / 4.0 )
        result = "sound/cart/andy/yeah-3.ogg";
      else if ( sound_selector < 3.0 / 4.0 )
        result = "sound/cart/andy/whohoo.ogg";
      else
        result = "sound/cart/andy/ah-ah.ogg";
    }
  else
    result = "sound/cart/andy/whoooooo.ogg";

  return result;
} // cart::get_combo_sample_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Export the methods of the class.
 */
void rp::cart::init_exported_methods()
{
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_idle, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_move, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_jump, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_fall, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_crouch, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_dead, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_with_tar, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, start_takeoff, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, regenerate, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, check_takeoff, void );
  TEXT_INTERFACE_CONNECT_METHOD_3
    ( cart, on_painter_collision, void, bear::engine::base_item&,
      bear::engine::base_item&, bear::universe::collision_info& );

  TEXT_INTERFACE_CONNECT_METHOD_0( cart, apply_crouch, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, apply_stop_crouch, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, jump, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, set_passive, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, throw_plunger, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, throw_cannonball, void );
  TEXT_INTERFACE_CONNECT_METHOD_0( cart, remove_elements, void );
} // cart::init_exported_methods()

/*----------------------------------------------------------------------------*/
TEXT_INTERFACE_IMPLEMENT_METHOD_LIST( rp::cart )
