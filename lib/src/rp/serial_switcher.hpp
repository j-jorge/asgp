/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a serial switcher.
 * \author Sebastien Angibaud
 */
#ifndef __RP_SERIAL_SWITCHER_HPP__
#define __RP_SERIAL_SWITCHER_HPP__

#include "generic_items/mouse_detector.hpp"
#include "engine/export.hpp"
#include "engine/item_brick/basic_renderable_item.hpp"
#include "visual/animation.hpp"

#include <claw/tween/tweener_sequence.hpp>

namespace rp
{
  /**
   * \brief The class describing a serial switcher.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class serial_switcher :
    public bear::engine::basic_renderable_item<bear::mouse_detector>
  {
    DECLARE_BASE_ITEM(serial_switcher);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::basic_renderable_item< bear::mouse_detector > super;

  public:
    serial_switcher();
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    bool set_u_integer_field
    ( const std::string& name, unsigned int value );
    bool set_animation_field
    ( const std::string& name, const bear::visual::animation& value );

    void get_visual
    ( std::list<bear::engine::scene_visual>& visuals ) const;

    bool mouse_released
    ( bear::input::mouse::mouse_code button,
      const claw::math::coordinate_2d<unsigned int>& pos );
    bool mouse_move( const claw::math::coordinate_2d<unsigned int>& pos );
    bool finger_action( const bear::input::finger_event& event );

    void create_angle_tweener();
    void on_angle_change( double angle );
    bool is_visible() const;    

  private:
    void render_star
    ( std::list<bear::engine::scene_visual>& visuals ) const;

    void update_serials() const;
    void check_mouse_inside
    ( const claw::math::coordinate_2d<unsigned int>& pos );

  private:
    /* \brief The serial of the switcher. */
    unsigned int m_serial;    

    /* \brief The next serial of the switcher. */
    unsigned int m_next_serial;

    /** \brief Indicates if the mouse is in the item at the last iteration. */
    bool m_mouse_in;

    /** \brief The sprite when the mouse is in the item. */
    bear::visual::animation m_animation_on;  
    
    /** \brief The sprite when the mouse is out the item. */
    bear::visual::animation m_animation_off;  

    /** \brief The angle of the arrow. */
    double m_angle; 
 
    /** \brief The tweener for angle. */
    claw::tween::tweener_sequence m_angle_tweener;

    bear::visual::sprite m_star;
    
  }; // class serial_switcher
} // namespace rp

#endif // __RP_SERIAL_SWITCHER_HPP__
