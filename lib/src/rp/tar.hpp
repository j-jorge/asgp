/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a tar.
 * \author Sebastien Angibaud
 */
#ifndef __RP_TAR_HPP__
#define __RP_TAR_HPP__

#include "rp/entity.hpp"

#include "engine/model.hpp"
#include "engine/base_item.hpp"

namespace rp
{
  /**
   * \brief The class describing a tar.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class tar :
    public bear::engine::model< bear::engine::base_item >,
    public entity
  {
    DECLARE_BASE_ITEM(tar);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model< bear::engine::base_item > super;
  
    TEXT_INTERFACE_DECLARE_METHOD_LIST(super, init_exported_methods)
  
  public:
    tar();
    
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );

    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void fall();
    void explose();

  protected:
    void populate_loader_map( bear::engine::item_loader_map& m );

  private:
    bool collision_with_plank(bear::engine::base_item& that);
    bool collision_with_cart(bear::engine::base_item& that);
    void finish_explose();
    
  private:
    static void init_exported_methods();

  private:
    /** \brief Tells if the tar was in contact with the cart at the last
        collision check. */
    bool m_cart_contact;

  }; // class tar
} // namespace rp

#endif // __RP_TAR_HPP__
