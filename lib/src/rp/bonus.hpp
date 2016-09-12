/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief A bonus for the cart.
 * \author Julien Jorge
 */
#ifndef __RP_BONUS_HPP__
#define __RP_BONUS_HPP__

#include "engine/base_item.hpp"
#include "engine/model.hpp"
#include "engine/export.hpp"

#include <claw/tween/tweener_sequence.hpp>

namespace rp
{
  class cart;

  /**
   * \brief A bonus for the cart.
   * The valid fields for this item are
   *  - \a type: (string) \b [required] the type of the bonus
   *      - "plunger"
   *      - "cart_element"
   *  - any field supported by the parent classes.
   *
   * \author Julien Jorge
   */
  class bonus:
    public bear::engine::model<bear::engine::base_item>
  {
    DECLARE_BASE_ITEM(bonus);

  public:
    /** \brief The type of the parent class. */
    typedef
    bear::engine::model<bear::engine::base_item> super;

    enum bonus_type
      {
        unknown_type = 0,
        plunger,
        cart_element
      }; // enum bonus_type

  public:
    bonus();
    void pre_cache();
    void on_enters_layer();
    void progress( bear::universe::time_type elapsed_time );
    bool set_string_field
    ( const std::string& name, const std::string& value );
    bool is_valid() const;

    static bonus_type type_from_string( const std::string& t );
    static std::string type_to_string( bonus_type t );

  private:
    void collision
    ( bear::engine::base_item& that, bear::universe::collision_info& info );
    void give_bonus( cart* c );

    void on_y_position_change(const bear::universe::coordinate_type& value);
    void on_x_position_change(const bear::universe::coordinate_type& value);

  private:
    /* \brief Indicates the type of the bonus. */
    bonus_type m_type;

    /* \brief Indicates the the bonus has been given. */
    bool m_bonus_is_given;

    /** \brief A pointer on cart. */
    cart* m_cart;    

    /** \brief The tweener for x-coordinate. */
    claw::tween::tweener_sequence m_tweener_x_position;

    /** \brief The tweener for y-coordinate. */
    claw::tween::tweener_sequence m_tweener_y_position;
  }; // class bonus
} // namespace rp

#endif // __RP_BONUS_HPP__
