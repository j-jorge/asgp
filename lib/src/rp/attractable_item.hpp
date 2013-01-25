/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.
*/
/**
 * \file
 * \brief The class describing an item that can be attracted.
 * \author Sebastien Angibaud
 */
#ifndef __RP_ATTRACTABLE_ITEM_HPP__
#define __RP_ATTRACTABLE_ITEM_HPP__

#include "rp/entity.hpp"

#include "engine/model.hpp"
#include "engine/base_item.hpp"

namespace rp
{
  class cart;
  class plunger;

  /**
   * \brief The class describing an itel that can be attracted.
   *
   * The valid fields for this item are
   *  - any field supported by the parent classes.
   *
   * \author Sebastien Angibaud
   */
  class attractable_item :
    public bear::engine::model< bear::engine::base_item >,
    public entity
  {
  public:
    /** \brief The type of the parent class. */
    typedef bear::engine::model< bear::engine::base_item > super;

  public:
    attractable_item();
    virtual void attract(plunger* p);
    virtual void be_taken(cart* c);
    virtual std::string get_type() const = 0;
    bool get_attracted_state() const;
    bool get_taken_state() const;
    virtual bool is_attractable() const;
    
  protected:
    void leave();
    void populate_loader_map( bear::engine::item_loader_map& m );

  private:
    /** \brief Indicates if the item has been attracted. */
    bool m_attracted;
    
    /** \brief Indicates if the item has been taken. */
    bool m_taken;

    /** \brief The plunger that attracts the object. */
    plunger* m_plunger;

  }; // class attractable_item
} // namespace rp

#endif // __RP_ATTRACTABLE_ITEM_HPP__
