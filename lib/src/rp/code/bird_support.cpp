/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the bird_support class.
 * \author Sebastien Angibaud
 */

#include "rp/bird_support.hpp"
#include "rp/cart.hpp"

#include "universe/collision_info.hpp"
#include "universe/zone.hpp"

BASE_ITEM_EXPORT( bird_support, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::bird_support::bird_support()
{
  set_artificial(false);
} // rp::bird_support::bird_support()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::bird_support::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  super::collision(that, info);

  collision_with_cart(that,info);
} // bird_support::collision()

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with a cart.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 * \return Return True if the collision is with a cart.
 */
bool rp::bird_support::collision_with_cart
( bear::engine::base_item& that, bear::universe::collision_info& info )
{ 
  bool result = false;
  cart* c = dynamic_cast<cart*>(&that);
  
  if ( c != NULL ) 
    {
      if ( info.get_collision_side() == bear::universe::zone::top_zone ) 
        c->jump();

      result = true;
    }
  
  return result;
} // bird_support::collision_with_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tells if it is interesting to process a collision against a given
 *        item.
 * \param that The other item.
 */
bool rp::bird_support::do_interesting_collision
( const bear::universe::physical_item& that ) const
{
  return dynamic_cast<const cart*>(&that) != NULL;
} // bird_support::do_interesting_collision()
