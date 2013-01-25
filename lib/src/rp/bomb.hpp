/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a bomb.
 * \author Sebastien Angibaud
 */
#ifndef __RP_BOMB_HPP__
#define __RP_BOMB_HPP__

#include "rp/attractable_item.hpp"

namespace rp
{
  /**
   * \brief The class describing a bomb.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class bomb :
    public attractable_item
  {
    DECLARE_BASE_ITEM(bomb);
    
  public:
    /** \brief The type of the parent class. */
    typedef attractable_item super;
  
    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
  public:
    bomb();
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );

    std::string get_type() const;
    void be_taken(cart * c);
    void attract( bear::engine::base_item& attractor );
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void explose();
    bool is_explosed() const;
    bool is_attractable() const;

  private:
    bool collision_with_cannonball(bear::engine::base_item& that);
    bool collision_with_explosion
    (bear::engine::base_item& that, bear::universe::collision_info& info);
    bool collision_with_obstacle(bear::engine::base_item& that);
    bool collision_with_cart(bear::engine::base_item& that);
    bool collision_with_wall(bear::engine::base_item& that);
    bool collision_with_zeppelin(bear::engine::base_item& that);
    bool collision_with_plank(bear::engine::base_item& that);
    bool collision_with_tar
    (bear::engine::base_item& that, bear::universe::collision_info& info);
    bool collision_with_bird(bear::engine::base_item& that);
    void finish_explose();
    void make_dirty();
    void create_decorations();
    void create_decorative_blast
    (const std::string& sprite_name, const bear::universe::speed_type& speed);

  private:
    static void init_exported_methods();

  private:
    /** \brief Indicates if the bomb has been explosed. */
    bool m_explosed;  

    /** \brief The initial mass. */
    static const double s_initial_mass;
  }; // class bomb
} // namespace rp

#endif // __RP_BOMB_HPP__
