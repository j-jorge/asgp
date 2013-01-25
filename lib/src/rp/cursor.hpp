/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing a cursor.
 * \author Sebastien Angibaud
 */
#ifndef __RP_CURSOR_HPP__
#define __RP_CURSOR_HPP__

#include "engine/base_item.hpp"
#include "engine/export.hpp"

#include "visual/sprite.hpp"

namespace rp
{
  /**
   * \brief The class describing a cursor.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class cursor :
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM(cursor);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::base_item super;

  public:
    cursor();

    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    bear::universe::position_type get_target() const;
    bool set_sprite_field
    ( const std::string& name, const bear::visual::sprite& value );
    void get_visual
    ( std::list<bear::engine::scene_visual>& visuals ) const;
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );

  private:
    /* \brief The position of the target. */
    bear::universe::position_type m_target_position;

    /** \brief The sprite of the picture. */
    bear::visual::sprite m_sprite;
  }; // class cursor
} // namespace rp

#endif // __RP_CURSOR_HPP__
