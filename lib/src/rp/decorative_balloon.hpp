/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a decorative balloon.
 * \author Sebastien Angibaud
 */
#ifndef __RP_DECORATIVE_BALLOON_HPP__
#define __RP_DECORATIVE_BALLOON_HPP__

#include "generic_items/decorative_item.hpp"

#include <claw/tween/single_tweener.hpp>
#include <claw/tween/tweener_sequence.hpp>

namespace rp
{
  class balloon;
  class cart;

  /**
   * \brief The class describing a decorative balloon.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class decorative_balloon :
    public bear::decorative_item
  {
    DECLARE_BASE_ITEM(decorative_balloon);
    
  public:
    /** \brief The type of the parent class. */
    typedef decorative_item super;
  
  public:
    decorative_balloon();
    decorative_balloon(const cart* c, const std::string& anchor_name );
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    void get_visual( std::list<bear::engine::scene_visual>& visuals ) const;
    void set_balloon( const balloon* b );
    void fly(double waiting, double y_distance, bool new_balloon);
    void free();
    void set_gap_z(int gap_z);

  private:
    void on_distance_change(const bear::universe::coordinate_type& value);
    void on_width_change(const bear::universe::coordinate_type& value);
    void on_height_change(const bear::universe::coordinate_type& value);
    void update_angle();

  private:
    /** \brief A pointer on cart. */
    const cart* m_cart;   

    /** \brief The name of the anchor for link. */
    std::string m_anchor_name;

    /** \brief Indicates if balloon flies (end of level). */
    bool m_fly;
    
    /** \brief Indicates if balloon is linked at cart. */
    bool m_is_linked;

    /** \brief The distance with anchor. */
    bear::universe::position_type m_distance;

    /** \brief The initial distance with anchor. */
    bear::universe::position_type m_initial_distance;

    /** \brief The duration before movement. */
    double m_waiting;

    /** \brief The duration since initialisation. */
    double m_duration;

    /** \brief The gap on z position. */
    int m_gap_z;

    /** \brief The tweener for flying. */
    claw::tween::tweener_sequence m_tweener_fly;

    /** \brief The tweener for balloon height. */
    claw::tween::tweener_sequence m_tweener_height;

    /** \brief The tweener for balloon width. */
    claw::tween::tweener_sequence m_tweener_width;
  }; // class decorative_balloon
} // namespace rp

#endif // __RP_DECORATIVE_BALLOON_HPP__
