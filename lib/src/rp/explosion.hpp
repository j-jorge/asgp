/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a explosion.
 * \author Sebastien Angibaud
 */
#ifndef __RP_EXPLOSION_HPP__
#define __RP_EXPLOSION_HPP__

#include "rp/entity.hpp"

#include "engine/base_item.hpp"
#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The class describing an explosion.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class explosion :
    public bear::engine::base_item,
    public entity
  {
    DECLARE_BASE_ITEM(explosion);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::base_item super;
  
  public:
    explosion();
    explosion
    (unsigned int nb_explosions, bear::universe::coordinate_type radius,
     double duration, bool decoration = false);
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    bool test_in_explosion(bear::universe::collision_info& info) const;

  private:
    bool collision_with_zeppelin
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

    void create_explosion();

    void create_smoke( std::size_t count ) const;
    void create_smoke() const;
    void apply_random_smoke_effect( bear::engine::base_item& item ) const;

    bear::universe::position_type
    pick_random_position_in_annulus
    ( bear::universe::coordinate_type r_min,
      bear::universe::coordinate_type r_max ) const;

  private:
    /* \brief Number of explosions to create. */
    unsigned int m_nb_explosions;
    
    /* \brief The explosion radius. */
    bear::universe::coordinate_type m_radius;

    /* \brief The duration since the explosion. */
    bear::universe::time_type m_duration;
    
    /** \brief The duration of the exlosion. */
    double m_explosion_duration;

  }; // class explosion
} // namespace rp

#endif // __RP_EXPLOSION_HPP__
