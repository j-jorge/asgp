/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::add_ingame_layers class.
 * \author Julien Jorge
 */
#include "rp/add_ingame_layers.hpp"

#include "rp/best_action_observer.hpp"
#include "rp/cart.hpp"
#include "rp/defines.hpp"
#include "rp/game_variables.hpp"
#include "rp/rp_gettext.hpp" 
#include "rp/transition_effect/level_starting_effect.hpp"

#include "engine/comic/layer/balloon_layer.hpp"
#include "engine/layer/transition_layer.hpp"
#include "engine/export.hpp"
#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "engine/world.hpp"

#include "rp/callback_queue.hpp"

#include "rp/layer/help_layer.hpp"
#include "rp/layer/key_layer.hpp"
#include "rp/layer/status_layer.hpp"
#include "rp/layer/misc_layer.hpp"
#include "rp/layer/pause_layer.hpp"

#ifndef NDEBUG
#include "generic_items/layer/physics_layer.hpp"
#include "generic_items/layer/link_layer.hpp"
#include "generic_items/layer/recent_path_layer.hpp"
#include "generic_items/layer/runtime_settings_layer.hpp"
#include "generic_items/layer/wireframe_layer.hpp"
#endif

BASE_ITEM_EXPORT( add_ingame_layers, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::add_ingame_layers::add_ingame_layers()
: m_add_starting_effect(true), m_add_status_layer(true), m_add_key_layer(false),
  m_level_timer(NULL)
{

} // add_ingame_layers::add_ingame_layers()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::add_ingame_layers::pre_cache()
{
  bear::engine::level_globals& globals( get_level_globals() );
  
  globals.load_image( rp_gettext("gfx/status/buttons.png") );
  globals.load_image( rp_gettext("gfx/status/buttons-2.png") );
  globals.load_image( rp_gettext("gfx/status/bonus.png") );

  globals.load_sound( "sound/medal/applause.ogg" );
  globals.load_sound( "sound/tick.ogg" );
  globals.load_sound( "sound/cart/andy/sad.ogg" );
  globals.load_sound( "sound/medal/bronze.ogg" );
  globals.load_sound( "sound/medal/silver.ogg" );
  globals.load_sound( "sound/medal/gold.ogg" );

  globals.load_font( "font/FrancoisOne.ttf" );
  globals.load_font( "font/LuckiestGuy.ttf" );
  
  // These may be optional for each level. We still load them everytime in order
  // for the user to endure the loading time at the beginning of the game.
  globals.load_model("model/balloon.cm");
  globals.load_model("model/bomb.cm");
  globals.load_model("model/tar.cm");
  globals.load_model("model/switching.cm");
  globals.load_model("model/wall.cm");
  globals.load_model("model/tnt.cm");
  globals.load_model("model/zeppelin/zeppelin.cm");
  globals.load_model("model/zeppelin/zeppelin-mirror.cm");

  globals.load_image( "gfx/aquatic/ground/ground-1.png" );
  globals.load_image( "gfx/aquatic/ground/ground-2.png" );
  globals.load_image( "gfx/aquatic/ground/ground-3.png" );
  globals.load_image( "gfx/wall-fill/aquatic/wall-fill-1.png" );
  globals.load_image( "gfx/wall-fill/aquatic/wall-fill-2.png" );

  globals.load_image( "gfx/cake/ground/ground-1.png" );
  globals.load_image( "gfx/cake/ground/ground-2.png" );
  globals.load_image( "gfx/cake/ground/ground-3.png" );
  globals.load_image( "gfx/wall-fill/cake/wall-fill-1.png" );
  globals.load_image( "gfx/wall-fill/cake/wall-fill-2.png" );
  globals.load_image( "gfx/wall-fill/cake/wall-fill-3.png" );

  globals.load_image( "gfx/death/ground/ground-1.png" );
  globals.load_image( "gfx/death/ground/ground-2.png" );
  globals.load_image( "gfx/death/ground/ground-3.png" );
  globals.load_image( "gfx/wall-fill/death/wall-fill-1.png" );
  globals.load_image( "gfx/wall-fill/death/wall-fill-2.png" );
  globals.load_image( "gfx/wall-fill/death/wall-fill-3.png" );

  globals.load_image( "gfx/garden/ground/ground-1.png" );
  globals.load_image( "gfx/garden/ground/ground-2.png" );
  globals.load_image( "gfx/garden/ground/ground-3.png" );
  globals.load_image( "gfx/wall-fill/garden/wall-fill-1.png" );
  globals.load_image( "gfx/wall-fill/garden/wall-fill-2.png" );
  globals.load_image( "gfx/wall-fill/garden/wall-fill-3.png" );

  globals.load_image( "gfx/space/ground/ground-1.png" );
  globals.load_image( "gfx/space/ground/ground-2.png" );
  globals.load_image( "gfx/space/ground/ground-3.png" );
  globals.load_image( "gfx/wall-fill/space/wall-fill-1.png" );
  globals.load_image( "gfx/wall-fill/space/wall-fill-2.png" );
  globals.load_image( "gfx/wall-fill/space/wall-fill-3.png" );

  globals.load_image( "gfx/western/ground/ground-1.png" );
  globals.load_image( "gfx/western/ground/ground-2.png" );
  globals.load_image( "gfx/western/ground/ground-3.png" );
  globals.load_image( "gfx/wall-fill/western/wall-fill-1.png" );
  globals.load_image( "gfx/wall-fill/western/wall-fill-2.png" );
  globals.load_image( "gfx/wall-fill/western/wall-fill-3.png" );

  static const std::array< unsigned int, 4 > digit_font_sizes
      {{ 25, 32, 40, 64 }};
  
  for ( unsigned int size : digit_font_sizes )
    {
      const bear::visual::font font
        ( globals.get_font( "font/LuckiestGuy.ttf", size ) );

      for ( char c : "0123456789" )
        font.get_sprite( c );
    }

  const bear::visual::font font
    ( globals.get_font( "font/LuckiestGuy.ttf", 32 ) );

  for ( char c : "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" )
    font.get_sprite( c );
} // add_ingame_layers::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the item.
 */
void rp::add_ingame_layers::build()
{
  new_item( *( new callback_queue() ) );

  bear::engine::transition_layer* transition
    ( new bear::engine::transition_layer
      (RP_TRANSITION_EFFECT_DEFAULT_TARGET_NAME) );
  
  get_level().push_layer
    ( new bear::engine::balloon_layer( RP_BALLOON_LAYER_DEFAULT_TARGET_NAME ) );

  get_level().push_layer( transition );
  
  if ( m_add_status_layer )
    {
      get_level().push_layer( new status_layer() );
      get_level().push_layer
        ( new pause_layer( RP_PAUSE_LAYER_DEFAULT_TARGET_NAME ) );
    }
    
  if ( m_add_key_layer )
    get_level().push_layer( new key_layer( RP_KEY_LAYER_DEFAULT_TARGET_NAME ) );
    
  get_level().push_layer
    ( new help_layer(RP_HELP_LAYER_DEFAULT_TARGET_NAME) );
  
  get_level().push_layer( new misc_layer() );  
  
#ifndef NDEBUG
  get_level().push_layer( new bear::link_layer );
  get_level().push_layer( new bear::physics_layer );
  get_level().push_layer( new bear::wireframe_layer );
  get_level().push_layer( new bear::recent_path_layer );
  get_level().push_layer
    ( new bear::runtime_settings_layer
      ( get_level_globals().get_font("font/FrancoisOne.ttf",25) ) );
#endif

  if (m_add_starting_effect)
    {
      transition->push_effect( new level_starting_effect, 1000 );
      new_item( *( new best_action_observer() ) );
    }
  else
    game_variables::set_level_starting(true);

  kill();
} // add_ingame_layers::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set a field of type \c boolean.
 * \param name The name of the field.
 * \param value The value of the field.
 */
bool
rp::add_ingame_layers::set_bool_field( const std::string& name, bool value )
{
  bool result(true);

  if ( name == "add_ingame_layers.show_introduction" )
    m_add_starting_effect = value;
  else if ( name == "add_ingame_layers.status_layer" )
    m_add_status_layer = value;
  else if ( name == "add_ingame_layers.key_layer" )
    m_add_key_layer = value;
  else
    result = super::set_bool_field(name, value);

  return result;
} // add_ingame_layers::set_bool_field()

/*---------------------------------------------------------------------------*/
/**
 * \brief Set the level timer.
 */
void rp::add_ingame_layers::set_level_timer(const bear::timer* level_timer)
{
  m_level_timer = level_timer;
} // add_ingame_layers::set_level_timer()
