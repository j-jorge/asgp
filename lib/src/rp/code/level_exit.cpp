/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the rp::level_exit class.
 * \author Sébastien Angibaud
 */
#include "rp/level_exit.hpp"
#include "rp/cart.hpp"

#include "rp/game_variables.hpp"
#include "engine/level.hpp"

BASE_ITEM_EXPORT( level_exit, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::level_exit::level_exit()
{
  set_phantom(true);
  set_can_move_items(false);
} // level_exit::level_exit()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the item.
 */
void rp::level_exit::build()
{
  super::build();

  game_variables::load_variables(get_level().get_name());
} // level_exit::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::level_exit::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);
  
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    c->apply_takeoff();
} // level_exit::collision()
