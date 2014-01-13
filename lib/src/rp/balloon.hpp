/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a balloon.
 * \author Sebastien Angibaud
 */
#ifndef __RP_BALLOON_HPP__
#define __RP_BALLOON_HPP__

#include "rp/attractable_item.hpp"

#include <claw/tween/single_tweener.hpp>

namespace rp
{
  class plunger;
  class cart;

  /**
   * \brief The class describing a balloon.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class balloon :
    public attractable_item
  {
    DECLARE_BASE_ITEM(balloon);
    
  public:
    /** \brief The type of the parent class. */
    typedef attractable_item super;
  
    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
  public:
    balloon();
    
    static std::string get_random_color();

    std::string get_color() const;

    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );

    std::string get_type() const;
    void be_taken(cart* c);
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void explose(bool counted = true);
    void attract(plunger* p);
    bool is_attractable() const;
    void fly(cart* c);

    bool has_attack_point() const;
    bear::universe::position_type get_attack_point() const;
    
  private:
    bool collision_with_cannonball(bear::engine::base_item& that);
    bool collision_with_cable(bear::engine::base_item& that);
    bool collision_with_explosion
    (bear::engine::base_item& that, bear::universe::collision_info& info);
    bool collision_with_balloon
    (bear::engine::base_item& that, bear::universe::collision_info& info );
    bool collision_with_plank(bear::engine::base_item& that );
    bool collision_with_tar(bear::engine::base_item& that );
    void finish_explose();
    void create_decorations();
    void create_decorative_blast
    (const std::string& sprite_name, const bear::universe::speed_type& speed);
    void create_tar_balloon();
    
    void on_y_position_change(const bear::universe::coordinate_type& value);

  private:
    static void init_exported_methods();

  private:
    /** \brief The color of this balloon. */
    std::string m_color;

    /** \brief Indicates if the balloon has been hit. */
    bool m_hit;

    /** \brief Indicates if the balloon flies. */
    bool m_fly;

    /** \brief A pointer on cart. */
    cart* m_cart;    

    /** \brief The tweener for y-coordinate. */
    claw::tween::single_tweener m_tweener_y_position;

    /** \brief Initial position of the balloon. */
    bear::universe::position_type m_initial_position;
  }; // class balloon
} // namespace rp

#endif // __RP_BALLOON_HPP__
