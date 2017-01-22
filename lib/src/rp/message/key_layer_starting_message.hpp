/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief A message that shows the key layer.
 * \author Julien Jorge
 */
#ifndef __RP_KEY_LAYER_STARTING_MESSAGE_HPP__
#define __RP_KEY_LAYER_STARTING_MESSAGE_HPP__

#include "rp/layer/key_layer.hpp"

#include "communication/typed_message.hpp"

namespace rp
{
  class key_layer;
  
  /**
   * \brief A message that starts the key layer.
   * \author Julien Jorge
   */
  class key_layer_starting_message :
    public bear::communication::typed_message<key_layer>
    {
    public:
      bool apply_to( key_layer& that );

    }; // class key_layer_starting_message
} // namespace rp

#endif // __RP_KEY_LAYER_STARTING_MESSAGE_HPP__
