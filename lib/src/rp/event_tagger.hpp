/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
#ifndef __RP_EVENT_TAGGER_HPP__
#define __RP_EVENT_TAGGER_HPP__

#include "engine/base_item.hpp"
#include "engine/item_brick/item_with_toggle.hpp"

#include "engine/export.hpp"

namespace rp
{
  class event_tagger :
    public bear::engine::item_with_toggle<bear::engine::base_item>
  {
    DECLARE_BASE_ITEM(event_tagger);
    
  public:
    typedef bear::engine::item_with_toggle<bear::engine::base_item> super;

  public:
    event_tagger();
    
    bool set_bool_field( const std::string& name, bool value );
    bool set_string_field
    ( const std::string& name, const std::string& value );
    bool set_string_list_field
    ( const std::string& name, const std::vector< std::string >& value );

  protected:
    void on_toggle_on( bear::engine::base_item* activator );
    void on_toggle_off( bear::engine::base_item* activator );

    void tag( bool value ) const;
    
  private:
    bool m_tag_level_info;
    std::string m_type;
    std::vector< std::string > m_properties;
  };
}

#endif
