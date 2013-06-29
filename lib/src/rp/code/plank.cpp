/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the plank class.
 * \author Sebastien Angibaud
 */
#include "rp/plank.hpp"
#include "engine/level_globals.hpp"

BASE_ITEM_EXPORT( plank, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::plank::plank()
: m_sprite_name("plank 1")
{
} // rp::plank::rp::plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param sprite_name The name of the sprite.
 */
rp::plank::plank(const std::string& sprite_name)
: m_sprite_name(sprite_name)
{
} // rp::plank::rp::plank()

/*----------------------------------------------------------------------------*/
/**
 * \brief Load the media required by this class.
 */
void rp::plank::pre_cache()
{
  super::pre_cache();

  get_level_globals().load_image("gfx/obstacle/obstacle.png");
} // rp::plank::pre_cache()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do post creation actions.
 */
void rp::plank::on_enters_layer()
{
  super::on_enters_layer();

  set_phantom(true);
  set_weak_collisions(true);
  set_mass(2);
  set_system_angle_as_visual_angle(true);

  set_animation
    ( get_level_globals().auto_sprite
      ( "gfx/obstacle/obstacle.png", m_sprite_name ) );
  auto_size();
  set_gap_x(10-get_width()/2);
  set_gap_y(10-get_height()/2);  
  set_size(20,20);
} // rp::plank::on_enters_layer()
