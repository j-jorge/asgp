/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief Implementation of the hole class.
 * \author Sebastien Angibaud
 */
#include "rp/hole.hpp"

#include "rp/cart.hpp"
#include "rp/util.hpp"

#include "universe/collision_info.hpp"
#include "universe/zone.hpp"

BASE_ITEM_EXPORT( hole, rp )

/*----------------------------------------------------------------------------*/
/**
 * \brief Process a collision with an other item.
 * \param that The other item of the collision.
 * \param info Some informations about the collision.
 */
void rp::hole::collision
( bear::engine::base_item& that, bear::universe::collision_info& info )
{
  cart* p = dynamic_cast<cart*>(&that);

  if ( p != NULL )
    {
      if ( ( info.get_collision_side() == 
	     bear::universe::zone::middle_right_zone) || 
	   ( info.get_collision_side() == 
	     bear::universe::zone::middle_left_zone ) )
	default_collision(info);
      
      if ( p->get_current_action_name() != "dead" )
        {
          if ( get_width() < 100 )
            {
              util::create_hit_star
                ( *this, bear::visual::color_type("#e0e0e0"),
                  bear::visual::color_type("#12ab00"), 1 );
              p->die(false, true);
            }
          else
            p->die(false, false);
        }
    }
  else
    super::collision(that,info);
} // hole::collision()

