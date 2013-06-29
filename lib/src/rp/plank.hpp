/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief The class describing a plank.
 * \author Sebastien Angibaud
 */
#ifndef __RP_PLANK_HPP__
#define __RP_PLANK_HPP__

#include "rp/entity.hpp"

#include "engine/item_brick/item_with_decoration.hpp"
#include "engine/item_brick/basic_renderable_item.hpp"
#include "engine/base_item.hpp"
#include "engine/export.hpp"

namespace rp
{
  /**
   * \brief The class describing a plank.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class plank :
    public bear::engine::item_with_decoration
  < bear::engine::basic_renderable_item
    < bear::engine::base_item > >,
    public entity
  {
    DECLARE_BASE_ITEM(plank);
    
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::item_with_decoration
    < bear::engine::basic_renderable_item
       < bear::engine::base_item > > super;

  public:
    plank();
    plank(const std::string& sprite_name);

    void pre_cache();
    void on_enters_layer();

  private:
    /* \brief The name of the sprite. */
    std::string m_sprite_name;
  }; // class plank
} // namespace rp

#endif // __RP_PLANK_HPP__
