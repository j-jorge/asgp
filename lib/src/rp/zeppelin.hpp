/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a hot air balloon.
 * \author Sebastien Angibaud
 */
#ifndef __RP_ZEPPELIN_HPP__
#define __RP_ZEPPELIN_HPP__

#include "rp/entity.hpp"

#include "engine/base_item.hpp"
#include "engine/model.hpp" 

#include "engine/item_brick/item_with_toggle.hpp"

#include "universe/derived_item_handle.hpp"

namespace rp
{
  /**
   * \brief The class describing a hot air balloon.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class zeppelin :
    public bear::engine::model
  < bear::engine::item_with_toggle
    < bear::engine::base_item > >,
    public entity
  {
    DECLARE_BASE_ITEM(zeppelin);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model
    < bear::engine::item_with_toggle
      < bear::engine::base_item > > super;

    /** \brief The type of an handle on a base_item. */
    typedef
    bear::universe::derived_item_handle<bear::engine::base_item> handle_type;

    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
  public:
    zeppelin();
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    bool set_item_field( const std::string& name, base_item* value );
    void set_drop_item(base_item* drop_item);
    base_item* get_drop_item();

    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void explose();
    bool get_hit() const;
    void drop();
    virtual void destroy();

  protected:
    void populate_loader_map( bear::engine::item_loader_map& m );

  private:
    bool collision_with_cannonball(bear::engine::base_item& that);
    bool collision_with_cart(bear::engine::base_item& that);
    bool collision_with_plank(bear::engine::base_item& that);
    bool collision_with_plunger(bear::engine::base_item& that);
    bool collision_with_tar
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void finish_explose();
    void create_item();
    void create_default_movement();
    void make_dirty();
    
    void on_toggle_on( bear::engine::base_item* activator );
    void on_screen_collision
    ( bear::engine::base_item& mark, bear::engine::base_item& that,
      bear::universe::collision_info& info );

  private:
    static void init_exported_methods();

  private:
    /** \brief Indicates if the zeppelin has been hit. */
    bool m_hit;

    /** \brief The item that is copied. */
    bear::engine::base_item* m_item;

    /** \brief The item to drop. */
    handle_type m_drop_item;

  }; // class zeppelin
} // namespace rp

#endif // __RP_ZEPPELIN_HPP__
