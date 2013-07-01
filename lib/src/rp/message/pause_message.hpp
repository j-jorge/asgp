/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief A message to put the level in pause.
 * \author Julien Jorge
 */
#ifndef __RP_PAUSE_MESSAGE_HPP__
#define __RP_PAUSE_MESSAGE_HPP__

#include "communication/typed_message.hpp"

namespace rp
{
  class pause_layer;
  
  /**
   * \brief A message to put the level in pause.
   * \author Julien Jorge
   */
  class pause_message :
    public bear::communication::typed_message<pause_layer>
    {
    public:
      bool apply_to( pause_layer& that );

    }; // class pause_message
} // namespace rp

#endif // __RP_PAUSE_MESSAGE_HPP__
